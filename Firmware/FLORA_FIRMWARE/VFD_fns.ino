void IRAM_ATTR shiftSetValue(uint8_t pin, bool value) {
  //(value) ? bitSet(bytes[pinsToRegisterMap[pin]], pinsToBitsMap[pin]) : bitClear(bytes[pinsToRegisterMap[pin]], pinsToBitsMap[pin]);
  (value) ? bitSet(bytes[pin / 8], pin % 8) : bitClear(bytes[pin / 8], pin % 8);
}


/*
  void IRAM_ATTR shiftSetAll(bool value) {
  for (int i = 0; i < registersCount * 8; i++) {
    //(value) ? bitSet(bytes[pinsToRegisterMap[i]], pinsToBitsMap[i]) : bitClear(bytes[pinsToRegisterMap[i]], pinsToBitsMap[i]);
    (value) ? bitSet(bytes[i / 8], i % 8) : bitClear(bytes[i / 8], i % 8);
  }
  }
*/
void IRAM_ATTR shiftWriteBytes(volatile byte *data) {

  for (int i; i < registersCount; i++) {
     SPI.transfer(data[registersCount - 1 - i]);
  }

  // set gpio through register manipulation, fast!
  GPOS = 1 << LATCH;
  GPOC = 1 << LATCH;
}

void IRAM_ATTR  TimerHandler()
{

  // Only one ISR timer is available so if we want the dots to not glitch during wifi connection, we need to put it here...
  // speed of the dots depends on refresh frequency of the display
  if (enableDotsAnimation) {
    int stepCount = dotsAnimationSteps / registersCount;

    for (int i = 0; i < registersCount; i++) {
      if (dotsAnimationState >= stepCount * i && dotsAnimationState < stepCount * (i + 1)) {
        #if defined(CLOCK_VERSION_IV12)
        segmentBrightness[i][4] = bri_vals_separate[bri][i];
        #else
        segmentBrightness[i][7] = bri_vals_separate[bri][i];
        #endif
      } else {
        #if defined(CLOCK_VERSION_IV12)
        segmentBrightness[i][4] = 0;
        #else
        segmentBrightness[i][7] = 0;
        #endif
      }
    }
    dotsAnimationState++;
    if (dotsAnimationState >= dotsAnimationSteps) dotsAnimationState = 0;
  }

	if (enableShowDateDots)
	{
		#if defined(CLOCK_VERSION_IV12)
		segmentBrightness[1][4] = bri_vals_separate[bri][0];
		segmentBrightness[3][4] = bri_vals_separate[bri][2];
		#else
		segmentBrightness[1][7] = bri_vals_separate[bri][0];
	  segmentBrightness[3][7] = bri_vals_separate[bri][2];
		#endif
	}
	
	
  // Normal PWM
  for (int i = 0; i < registersCount; i++) {
    for (int ii = 0; ii < segmentCount; ii++) {
      if (isPoweredOn && shiftedDutyState[i] < segmentBrightness[i][ii]) {
        shiftSetValue(digitPins[i][ii], true);
      } else {
        shiftSetValue(digitPins[i][ii], false);
      }
    }
  }

  shiftWriteBytes(bytes); // Digits are reversed (first shift register = last digit etc.)

  for (int i = 0; i < registersCount; i++) {
    shiftedDutyState[i]++;
    if (shiftedDutyState[i] >= pwmResolution) {
      shiftedDutyState[i] = 0;
    }
  }

  //if (dutyState > pwmResolution) dutyState = 0;
  //else dutyState++;
}

void initScreen() {
  Serial.println("Start initScreen ");
  pinMode(DATA, INPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(LATCH, OUTPUT);
  digitalWrite(LATCH, LOW);
  Serial.println("Start initScreen ");

  bri = json["bri"].as<int>();
	colon = json["colon"].as<int>();
  setupBriBalance();
  Serial.println("OK setupBriBalance ");

  crossFadeTime = json["fade"].as<int>();
   Serial.println("OK crossFadeTime " + String(crossFadeTime));
   ///
  setupPhaseShift();
  Serial.println("OK setupPhaseShift ");

  setupCrossFade();
  Serial.println("OK setupCrossFade ");

  Serial.println("Start  SPI.begin ");
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  Serial.println("OK SPI.. ");

  ITimer.attachInterruptInterval(TIMER_INTERVAL_uS, TimerHandler);
  // 30ms slow 240ms, 20 => medium 160ms, 15 => quick 120ms
  Serial.println("OK ITimer.attachInterruptInterval ");

  blankAllDigits();
  Serial.println("OK blankAllDigits ");
}

void enableScreen() {
  ITimer.attachInterruptInterval(TIMER_INTERVAL_uS, TimerHandler);
}

void disableScreen() {
  ITimer.detachInterrupt();
}

void setupBriBalance() {
  if (json["bal_enable"].as<int>() == 0) return;
  for (int i = 0; i < registersCount; i++) {
    bri_vals_separate[0][i] = json["bal"]["low"][i].as<int>() -1; // low bri balance Steps are2 Values are 2 so -1 get 1,3,5 ...lowest value possible
    bri_vals_separate[1][i] = json["bal"]["medium"][i].as<int>(); // medium brightness is ignored and set to high
    bri_vals_separate[2][i] = json["bal"]["high"][i].as<int>(); // high bri balance
  }
}

void setupCrossFade() {
  if (crossFadeTime > 0) {
    fade_animation_ticker.attach_ms(crossFadeTime, handleFade); // handle dimming animation
  } else {
    fade_animation_ticker.attach_ms(20, handleFade); // handle dimming animation
  }
}
void handleFade() {
  for (int i = 0; i < registersCount; i++) {
    for (int ii = 0; ii < segmentCount; ii++) {
      if (targetBrightness[i][ii] > bri_vals_separate[bri][i]) targetBrightness[i][ii] = bri_vals_separate[bri][i];

      if (crossFadeTime > 0) {
        if (targetBrightness[i][ii] > segmentBrightness[i][ii]) {
          segmentBrightness[i][ii] += bri_vals_separate[bri][i] / dimmingSteps;
          if (segmentBrightness[i][ii] > bri_vals_separate[bri][i]) segmentBrightness[i][ii] = bri_vals_separate[bri][i];
        } else if (targetBrightness[i][ii] < segmentBrightness[i][ii]) {
          segmentBrightness[i][ii] -= bri_vals_separate[bri][i] / dimmingSteps;
          if (segmentBrightness[i][ii] < 0) segmentBrightness[i][ii] = 0;
        }
      } else {
        segmentBrightness[i][ii] = targetBrightness[i][ii];
      }
    }
  }
}

void setDigit(uint8_t digit, uint8_t value) {
  draw(digit, numbers[value]);
}

void setAllDigitsTo(uint16_t value) {
  for (int i = 0; i < registersCount; i++) {
    setDigit(i, value);
  }
}

void blankDigit(uint8_t digit) {
  for (int i = 0; i < sizeof(digitPins[digit]); i++) {
    targetBrightness[digit][i] = 0;
  }
}

void blankAllDigits() {
  for (int i = 0; i < registersCount; i++) {
    blankDigit(i);
  }
}

void setDot(uint8_t digit, bool enable) {
#ifndef CLOCK_VERSION_IV12
  if (enable) {
    targetBrightness[digit][7] = bri_vals_separate[bri][digit];
  } else {
    targetBrightness[digit][7] = 0;
  }
#endif
}

void draw(uint8_t digit, uint8_t value[segmentCount]) {
  for (int i = 0; i < segmentCount; i++) {
    if (value[i] == 1) {
      targetBrightness[digit][i] = bri_vals_separate[bri][digit];
    } else {
      targetBrightness[digit][i] = 0;
    }
  }
}

void showTime() {

  int hours = hour();
  if (hours > 12 && json["t_format"].as<int>() == 0) 
  { // 12 / 24 h format
    hours -= 12;
  } 
  else if 
  (hours == 0 && json["t_format"].as<int>() == 0) {
    hours = 12;
  }

  int splitTime[6] =   {
    (hours / 10) % 10,
    hours % 10,
    (minute() / 10) % 10,
    minute() % 10,
    (second() / 10) % 10,
    second() % 10,
  };

  for (int i = 0; i < registersCount ; i++) 
  {
    if (i == 0 && splitTime[0] == 0 && json["zero"].as<int>() == 0) 
    {
      blankDigit(i);
      continue;
    }
    setDigit(i, splitTime[i]);
  }

}

void showDate() {

  int splitDate[6] = 
  {
    (day() / 10) % 10,
    day() % 10,
    (month() / 10) % 10,
    month() % 10,
    0 ,0
  };

  for (int i = 0; i < registersCount ; i++) 
  {
    if (i == 0 && splitDate[0] == 0 && json["zero"].as<int>() == 0) 
    {
      blankDigit(i);
      continue;
    }
    setDigit(i, splitDate[i]);
  }
}

void showYear() {

  int yearx = year();
  int splitYear[6] = 
  {  
    (yearx / 1000) % 10,
    (yearx % 1000)/100,
    (yearx % 100) / 10,
    yearx % 10,
    0,0
  };

  for (int i = 0; i < registersCount ; i++) 
  {
    if (i == 0 && splitYear[0] == 0 && json["zero"].as<int>() == 0) 
    {
      blankDigit(i);
      continue;
    }
    setDigit(i, splitYear[i]);
  }
}

// shows one digit at the passed position 1,2,3,4 
void showDigit(int pos, int digit) {
  if(pos <= registersCount){
    blankDigit(pos-1);
    if(digit >= 0 && digit <= 9)
    {
      setDigit(pos-1, digit);
    }
  }
}

// shows 4 digit at position 1, 2, 3, 4
void showDigits(int digit1, int digit2 , int digit3, int digit4) {
  showDigit(1, digit1);
  showDigit(2, digit2);
  showDigit(3, digit3);
  showDigit(4, digit4);
}

void cycleDigits() {
  updateColonColor(azure[bri]);
  strip.Show();

  for (int i = 0; i < 10; i++) {
    for (int ii = 0; ii < registersCount; ii++) {
      setDigit(ii, i);
    }
    delay(1000);
  }

  strip.ClearTo(RgbColor(0, 0, 0));
  strip.Show();
}

void showIP(int delay_ms) {
  IPAddress ip_addr = WiFi.localIP();

  if (registersCount < 6) {
    blankDigit(0);
    if ((ip_addr[3] / 100) % 10 == 0) {
      blankDigit(1);
    } else {
      setDigit(1, (ip_addr[3] / 100) % 10);
    }
    if ((ip_addr[3] / 10) % 10 == 0) {
      blankDigit(2);
    } else {
      setDigit(2, (ip_addr[3] / 10) % 10);
    }
    setDigit(3, (ip_addr[3]) % 10);
  } else {
    setDigit(0, 1);
    draw(1, letter_p);
    blankDigit(2);
    if ((ip_addr[3] / 100) % 10 == 0) {
      blankDigit(3);
    } else {
      setDigit(3, (ip_addr[3] / 100) % 10);
    }
    if ((ip_addr[3] / 10) % 10 == 0 && (ip_addr[3] / 100) % 10 == 0) {
      blankDigit(4);
    } else {
      setDigit(4, (ip_addr[3] / 10) % 10);
    }
    setDigit(5, (ip_addr[3]) % 10);
  }

  updateColonColor(azure[bri]);
  strip_show();
  delay(delay_ms);
  strip.ClearTo(RgbColor(0, 0, 0));
  strip_show();
}

void setupPhaseShift() {
  disableScreen();
  uint8_t shiftSteps = floor(pwmResolution / registersCount);
  if (shiftSteps)
    for (int i = 0; i < registersCount; i++) {
      shiftedDutyState[i] = i * shiftSteps;
    }
  enableScreen();
}

void toggleNightMode() {
  // if togglenmode then activate NightMode
  // but at 6 o'clock deactivate the toggle
  int nmode = json["nmode"].as<int>(); 
	int nmodeoff = json["nmodeoff"].as<int>(); 
  if (togglenmode > 0)
  { 
    bri = 0;
    colon = 0;
    if( hour() == (nmodeoff + 5) && minute() == 0 )
    {
      togglenmode = 0;
    }
    return;
  }

  if (nmode > 0 && (hour() >= (nmode + 18)|| hour() <= (nmodeoff + 5))) {
    bri = 0;
    colon = 0;
  } else {
    bri = json["bri"].as<int>();
    colon = json["colon"].as<int>();
  }
}

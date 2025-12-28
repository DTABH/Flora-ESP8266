

// Menu starting with long pressed button
// 1 -> Setting minute,hour,year,month,day  Buttons 1->next setting, 2-> +1 and 3-> -1
// 2 ->Setting Show Date "5O", nightmode On "O8", nightmode Off "O1"  Buttons 2->next setting, 1-> +1 and 3-> -1
// 3 -> toggle for nightmode
// 1,2 together  Set RTC_Only true -> no WLAN no webpage no ntp
// 1,3 together  Set RTC_Only false-> access via WLAN  webpage and if configured ntp
// 1,2,3 together  Delete configuration and restart opening Accesspoint at 192.168.254.1 with Webpage 
void menue(int buttonpressed)
{

  Serial.println("menue " + String(buttonpressed));
  int endreached  = 0;

  // set coloc red during menu input
  RgbColor oldcolonColor = colonColor;
  int oldcolon = colon;
  colon = 1;
  colonColor =   red[bri];
  updateColonColor( red[bri]);
  strip_show();


  bool OldRTC_Only = RTC_Only;

  // Button 1 and 2 and3 clear save values restart and open webpage
  if (buttonpressed == 7)
  {
    deleteConfig();
    delay(1000);
    ESP.restart();
  }

  // Button 1 and 2 pressed together RTC_Only set to true
  if (buttonpressed == 3)
  { 
    if (RTC_Exists)
    {
      RTC_Only  = true;    
      readConfig();
      json["rtconly"] = 1;
      Serial.println("Set and Save RTC_Only " + String(RTC_Only));
      saveConfig();
      delay(1000);
      ESP.restart();
    }
    else
    {
      Serial.println("RTC not exists RTC_Only not set." );
    }
  }

  // Button 1 and 3 pressed together RTC_Only set to false
  if (buttonpressed == 5)
  { 
    RTC_Only  = false;
    readConfig();
    json["rtconly"] = 0;
    Serial.println("Set and Save RTC_Only " + String(RTC_Only));
    saveConfig();
    delay(1000);
    ESP.restart();
  }

  // button_3 pressed two times is toggle for temporary nightmode 
  // toogle force nightmode until next day
  if (buttonpressed == 4)
  {        
   
    // Loop over button pressings
    endreached = 0;    
    while (endreached < 30)
    {  
      if (digitalRead(BUTTON_3) )
      {
        if (togglenmode)
        {
          togglenmode = 0;
          }
        else
        {
          togglenmode = 1;
        }

        break; // toggle leave at once
      } 
      delay(200);
      endreached +=1;         
    }
    showTime(); // refresh Digits
    Serial.println("menue item : toogle force nightmode  togglenmode : " + String(togglenmode) );
  }            



  // don't try contact NTP Server
  RTC_Only = true;

  // minutes , hours , day, month, year
  if (buttonpressed == 1)
  {
    
    // 1= minutes , 2=hours , 3=year, 4=month, 5=day
    for (int i = 1; i < 6; i ++) 
    {      

      Serial.println("menue item : " + String(i));
       // Synch every 100 ms so changes can be shown at once
      setSyncInterval(0.1);
      delay(200);
      // Show the right menu
      if(i == 4 || i ==5)
      {
        showDate();
      }
      else if(i == 3)
      {
        showYear();
      }
      else
      {
        showTime();
      }
      setSyncInterval(3600);

      // Loop over button pressings
      endreached = 0;
      while (endreached < 30)
      { 
        if (analogRead(BUTTON_2) > 100)
        {
          changeTime(i, 1);
          endreached =0;
        }

        if (digitalRead(BUTTON_3) )
        {
          changeTime(i, -1);
          endreached =0;
        } 

        if (digitalRead(BUTTON_1) )
        { 
          delay(500);
          break;
        } 

        setSyncInterval(0.1);
        delay(200);
        endreached +=1;    
        // show changes in the right menu
        if(i == 4 || i ==5)
        {
          showDate();
        }
        else if(i == 3)
        {
          showYear();
        }
        else
        {
          showTime();
        }
        setSyncInterval(3600);
      }

      if (endreached > 28)
      {
        break;
      }     
    }    
  }

   // show date , 
  if (buttonpressed == 2)
  {    
    // 1 = show date, 2 = night mode , 3 = night mode off
    for (int i = 1; i < 4; i ++) 
    {      

      Serial.println("menue item : " + String(i));
       // Synch every 100 ms so changes can be shown at once
      setSyncInterval(0.1);
      delay(200);
      // Show the right menu
      // 5 = "S"  0 = "D" for "S"how"D"ate
      if(i == 1){showDigits(5,0,-1, showdate);}   
      // 0 = "O"  8 = "N"   ON 
      if(i == 2){showDigits(0,8,-1, nmode);}
      // 0 = "O"  1 = "F"   OFF       
      if(i == 3){showDigits(0,1,-1, nmodeoff);}      
      setSyncInterval(3600);

      // Loop over button pressings
      endreached = 0;
      while (endreached < 30)
      { 
        if (digitalRead(BUTTON_3))
        {
          if(i == 1){ if (showdate < 2) { showdate++; }}
          if(i == 2){ if (nmode < 5) { nmode++; }}
          if(i == 3){ if (nmodeoff < 2) { nmodeoff++; }}
          endreached =0;
        }

        if (digitalRead(BUTTON_1) )
        {
          if(i == 1){ if (showdate > 0) {showdate--;}}
          if(i == 2){ if (nmode > 0) {nmode--;}}
          if(i == 3){ if (nmodeoff > 0) {nmodeoff--;}}
          endreached =0;
        } 

        if (analogRead(BUTTON_2) > 100 )
        { 
          // next setting
          delay(500);
          break;
        } 

        setSyncInterval(0.1);
        delay(200);
        endreached +=1;    
        // Show the right menu
        // 5 = "S"  0 = "D" for "S"how"D"ate
        if(i == 1){showDigits(5,0,-1, showdate);}   
        // 0 = "O"  8 = "N"   ON 
        if(i == 2){showDigits(0,8,-1, nmode);}
        // 0 = "O"  1 = "F"   OFF       
        if(i == 3){showDigits(0,1,-1, nmodeoff);}   


        setSyncInterval(3600);
      }

      if (endreached > 28)
      {
        break;
      }     
    }    
  }

  colon = oldcolon;
  colonColor = oldcolonColor;

  RTC_Only = OldRTC_Only;
  setSyncInterval(3600); 

}

// changes the date and time with simple addition substraction of seconds from unixtime
// due to lack of a FULL working datetime library in arduino with addMonth, AddDay... functions
// which garantie a the right calculation (leap year, 30 ,31 days in month ...)
// after minutes an hours, first the year then month and then day can be changed
// otherwise a the  change of Month that could change the day leads to  change the day again
void changeTime(int part, int step)
{      
  unsigned long  unixtime = RTC_now.unixtime();
  Serial.println("changeTime  " + String(part) + "  " + String(step) + " act. unixtime: " + String(unixtime) );
 
  switch(part)
  {
    case 1:
      unixtime += step*60;
      break;
       
    case 2:
      unixtime += step*3600;
      break;

    case 3:    
      unixtime += step*86400*365;
      break;

    case 4:
      unixtime += step*86400*30;
      break;

    case 5:
      unixtime += step*86400;
      break;
 
  }

  Serial.print("changeTime " + String(part) + "  " + String(step) + " new unixtime : " + String(unixtime) );
  rtc.adjust(DateTime(unixtime));

  // delay(500);
}




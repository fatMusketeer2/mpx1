int isDateValid(int month, int day, int year) {
  
  int valid = 1;
  int monthLength[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  
  if(isLeapYear(year)) monthLength[1] = 29;
  if(month < 1 || month > 12) valid = 0;
  else if(day < 1 || day > monthLength[month-1]) valid = 0;
    
  return valid;
  
}
    
int isLeapYear(int year) {
  
  int result;
    
  if((year%4) != 0) result = 0;
  else if ((year%400) == 0) result = 1;
  else if ((year%100) == 0) result = 0;
  else result = 1;
    
  return result;
    
}

char* getMonth(int month) {
  
  char* name;
  
  switch(month) {
    case 1: name = "January";
      break;
    case 2: name = "February";
      break;
    case 3: name = "March";
      break;
    case 4: name = "April";
      break;
    case 5: name = "May";
      break;
    case 6: name = "June";
      break;
    case 7: name = "July";
      break;
    case 8: name = "August";
      break;
    case 9: name = "September";
      break;
    case 10: name = "October";
      break;
    case 11: name = "November";
      break;
    case 12: name = "December";
      break;
  }
  return name;
  
}
void ShotPutParseData (){
  positionR = encoderRcount;
  positionL = encoderLcount;
  Serial.print(positionL);
  Serial.print(" || ");
  Serial.println(positionR);
}

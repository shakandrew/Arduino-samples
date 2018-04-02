void setup() {
    Serial.begin(9600);
    delay(2000);

    Serial.println("* Timer/Counter0");
    {
        Serial.print("TCCR0A = "); Serial.println(TCCR0A, BIN);
        Serial.print("TCCR0B = "); Serial.println(TCCR0B, BIN);
        Serial.print("TIMSK0 = "); Serial.println(TIMSK0, BIN);
        Serial.print("GTCCR = "); Serial.println(GTCCR, BIN);
        Serial.print("TCNT0 = "); Serial.println(TCNT0, BIN);
        Serial.print("OCR0A = "); Serial.println(OCR0A, BIN);
        Serial.print("OCR0B = "); Serial.println(OCR0B, BIN);
        Serial.print("TIFR0 = "); Serial.println(TIFR0, BIN);
    }
    Serial.println("* Timer/Counter1");
    {
        Serial.print("TCCR1A = "); Serial.println(TCCR1A, BIN);
        Serial.print("TCCR1B = "); Serial.println(TCCR1B, BIN);
        Serial.print("TCCR1C = "); Serial.println(TCCR1C, BIN);
        Serial.print("TCNT1L = "); Serial.println(TCNT1L, BIN);
        Serial.print("TCNT1H = "); Serial.println(TCNT1H, BIN);
        Serial.print("ICR1L = "); Serial.println(ICR1L, BIN);
        Serial.print("ICR1H = "); Serial.println(ICR1H, BIN);
        Serial.print("OCR1AL = "); Serial.println(OCR1AL, BIN);
        Serial.print("OCR1AH = "); Serial.println(OCR1AH, BIN);
        Serial.print("OCR1BL = "); Serial.println(OCR1BL, BIN);
        Serial.print("OCR1BH = "); Serial.println(OCR1BH, BIN);
        Serial.print("TIFR1 = "); Serial.println(TIFR1, BIN);
    }
    Serial.println("* Timer/Counter2");
    {
        Serial.print("TCCR2A = "); Serial.println(TCCR2A, BIN);
        Serial.print("TCCR2B = "); Serial.println(TCCR2B, BIN);
        Serial.print("TCNT2 = "); Serial.println(TCNT2, BIN);
        Serial.print("OCR2A = "); Serial.println(OCR2A, BIN);
        Serial.print("OCR2B = "); Serial.println(OCR2B, BIN);
        Serial.print("TIMSK2 = "); Serial.println(TIMSK2, BIN);
        Serial.print("TIFR2 = "); Serial.println(TIFR2, BIN);
        Serial.print("ASSR = "); Serial.println(ASSR, BIN);
        Serial.print("ASSR = "); Serial.println(ASSR, BIN);
    }    
    Serial.print("* Common for everyone GTCCR ="); Serial.print(GTCCR, BIN);
}
void loop() {
}



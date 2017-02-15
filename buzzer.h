#define BUZZER_PIN 4  //D2  PWM control MUSIC
#define OCTAVE_OFFSET 0 //buzzer
void buzzer_init(){
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  }
int notes[] = { 0,
                262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494,
                523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988,
                1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976,
                2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951
              };
const char *song1 = "Indiana:d=4,o=5,b=250:e,8p,8f,8g,8p,1c6,8p.,d,8p,8e,1f,p.,g,8p,8a,8b,8p,1f6,p,a,8p,8b,2c6,2d6,2e6,e,8p,8f,8g,8p,1c6,p,d6,8p,8e6,1f.6,g,8p,8g,e.6,8p,d6,8p,8g,e.6,8p,d6,8p,8g,f.6,8p,e6,8p,8d6,2c6";
const char *song2 = "Nokia:d=4,o=5,b=100:16e6,16e6,32p,8e6,16c6,8e6,8g6,8p,8g,8p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,16p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16c7,16p,16c7,16c7,p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16d#6,8p,16d6,8p,16c6";
#define isdigit(n) (n >= '0' && n <= '9')

void buzzer(int freq, int vol, int TL,int t_pause) {
  //freq 1~9:500Hz~1300Hz
  //vol 1~9:0~270/1023
  analogWriteFreq(400 + freq * 100);  // set PWM to 20Khz
  analogWrite(BUZZER_PIN, vol);
  delay(TL);
  
  analogWrite(BUZZER_PIN, 0);
  delay(t_pause);
}

void play_rtttl(const char *p, int vol, int play_speed) //if play_speed=0,use default
{
  // Absolutely no error checking

  byte default_dur = 4;
  byte default_oct = 6;
  int bpm = 63;
  int num;
  long wholenote;
  long duration;
  byte note;
  byte scale;

  // format: d=N,o=N,b=NNN:
  // find the start (skip name, etc)

  while (*p != ':') // ignore name
    p++;
  p++;                     // skip ':'

  // get default duration
  if (*p == 'd')
  {
    p++; p++;              // skip "d="
    num = 0;
    while (isdigit(*p)) {
      num = (num * 10) + (*p++ - '0');
    }
    if (num > 0)
      default_dur = num;
    p++;                   // skip comma
  }
  //Serial.print("default_dur: ");
  //Serial.println(default_dur, 10);

  // get default octave
  if (*p == 'o')
  {
    p++; p++;              // skip "o="
    num = *p++ - '0';
    if (num >= 3 && num <= 7)
      default_oct = num;
    p++;                   // skip comma
  }
  //Serial.print("default_oct: ");
  //Serial.println(default_oct, 10);

  // get BPM, beats per minute, tempo
  if (*p == 'b')
  {
    p++; p++;              // skip "b="
    num = 0;
    while (isdigit(*p)) {
      num = (num * 10) + (*p++ - '0');
    }
    bpm = num;
    p++;                   // skip ':'
  }
  //Serial.print("bpm: ");
  //Serial.println(bpm, 10);

  // BPM usually expresses the number of quarter notes per minute
  wholenote = (60 * 1000L / bpm) * 4;  // this is the time for whole note (in milliseconds)

  //Serial.print("wholenote: ");
  //Serial.println(wholenote, 10);

  // now begin note loop
  while (*p)
  {
    // first, get note duration, if available
    num = 0;
    while (isdigit(*p)) {
      num = (num * 10) + (*p++ - '0');
    }

    if (num)
      duration = wholenote / num;
    else
      duration = wholenote / default_dur;  // need to check if is a dotted note after

    // now get the note
    note = 0;

    switch (*p) {
      case 'c':
        note = 1;
        break;
      case 'd':
        note = 3;
        break;
      case 'e':
        note = 5;
        break;
      case 'f':
        note = 6;
        break;
      case 'g':
        note = 8;
        break;
      case 'a':
        note = 10;
        break;
      case 'b':
        note = 12;
        break;
      case 'p':
      default:
        note = 0;
    }
    p++;

    // now, get optional '#' sharp
    if (*p == '#') {
      note++;
      p++;
    }

    // now, get optional '.' dotted note
    if (*p == '.') {
      duration += duration / 2;
      p++;
    }

    // now, get scale
    if (isdigit(*p)) {
      scale = *p - '0';
      p++;
    }
    else {
      scale = default_oct;
    }
    scale += OCTAVE_OFFSET;

    if (*p == ',')
      p++;       // skip comma for next note (or we may be at the end)

    // now play the note

    if (play_speed > 0)duration = play_speed;

    if (note) {
      //Serial.print("Playing: ");
      //Serial.print(scale, 10); Serial.print(' ');
      //Serial.print(note, 10); Serial.print(" (");
      //Serial.print(notes[(scale - 4) * 12 + note], 10);
      //Serial.print(") ");
      //Serial.println(duration, 10);

      //tone(BUZZER_PIN, notes[(scale - 4) * 12 + note]);
      analogWriteFreq(notes[(scale - 4) * 12 + note]);  // set PWM to 20Khz
      analogWrite(BUZZER_PIN, vol);

      delay(duration);
      analogWrite(BUZZER_PIN, 0);
      //noTone(BUZZER_PIN);
    }
    else {
      //Serial.print("Pausing: ");
      //Serial.println(duration, 10);
      delay(duration);
    }
  }
}

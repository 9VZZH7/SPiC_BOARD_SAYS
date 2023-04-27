#include <led.h>
#include <7seg.h>
#include <adc.h>
#include <button.h>
#include <timer.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// Definieren eines platzsparenden 3-bit Datentyps zur effizienten Speicherung von Zahlen 0 bis 7 (entspr. LEDs)
typedef struct {
	unsigned led_pattern_i:3;
} led_pattern;

// Event Variable für Button Interrupt
static volatile uint8_t event = 0;

// Semi-Random LED-Sequenz, tatsächlich zufällige Zahlen auf dem Mikrocontroller sind schwer umsetzbar
static led_pattern random_sequence[32];


// Interrupt-handler für BUTTON0
ISR(INT0_vect){
	event = 1;
}

// Hilfsfunktion zum Anziegen einer Textnachricht auf der 7-seg Anzeige
static void show_Msg(void) {
	char msg[] = {"  SPIc-boArd SAYS   rEPEAt LEd cYcLE   USE POtI And bUtton 0   PrESS bUtton 0 to StArt  "};
	sb_timer_delay(70);
	char *c_p = msg;
	event = 0;
	// Bricht bei Interrupt ab oder falls String zu Ende
	sb_timer_delay(1000);
	while(!event&!(*c_p=='\0')){
		if(*(c_p+1)!='\0'){
			char tmp[3];
			tmp[0] = *c_p;
			tmp[1] = *(c_p+1);
			tmp[2] = '\0';
			sb_7seg_showString(tmp);
		}
	c_p = c_p + 1;
	sb_timer_delay(400);
	}
	event = 0;
}

// Hilfsfunktion zur Berechnung der Wurzel
static uint8_t sqrt(uint8_t a){
	uint8_t i = 0;
	while((i+1)*(i+1)<a){
		i = i+1;
	}
	return i;
}

// Gibt einen nahezu zufällig generierten Wert zwischen 0 und 8 zurück
static uint8_t random(uint8_t level, uint8_t i, uint8_t length){
	uint8_t salt = sb_adc_read(PHOTO);
	salt = ((salt<<4)>>4);
	return random_sequence[((length+(i+1)*level)+salt) %32].led_pattern_i + 1;

}


// Initialisierung
static void init(void){

	// Konfiguration der Hardware für die Benutzung des Buttons
	DDRD &= ~(1<<PD2);
	PORTD |= (1<<PD2);
	EICRA |= (1<<ISC00)|(1<<ISC01);
	EIMSK |= (1<<INT0);

	// Füllen des Arrays mit Werten
	random_sequence[0].led_pattern_i = 1;
	random_sequence[1].led_pattern_i = 4;
	random_sequence[2].led_pattern_i = 7;
	random_sequence[3].led_pattern_i = 2;
	random_sequence[4].led_pattern_i = 0;
	random_sequence[5].led_pattern_i = 5;
	random_sequence[6].led_pattern_i = 3;
	random_sequence[7].led_pattern_i = 6;
	random_sequence[8].led_pattern_i = 3;
	random_sequence[9].led_pattern_i = 2;
	random_sequence[10].led_pattern_i = 1;
	random_sequence[11].led_pattern_i = 6;
	random_sequence[12].led_pattern_i = 5;
	random_sequence[13].led_pattern_i = 4;
	random_sequence[14].led_pattern_i = 0;
	random_sequence[15].led_pattern_i = 7;
	random_sequence[16].led_pattern_i = 1;
	random_sequence[17].led_pattern_i = 4;
	random_sequence[18].led_pattern_i = 2;
	random_sequence[19].led_pattern_i = 3;
	random_sequence[20].led_pattern_i = 5;
	random_sequence[21].led_pattern_i = 7;
	random_sequence[22].led_pattern_i = 6;
	random_sequence[23].led_pattern_i = 0;
	random_sequence[24].led_pattern_i = 1;
	random_sequence[25].led_pattern_i = 6;
	random_sequence[26].led_pattern_i = 7;
	random_sequence[27].led_pattern_i = 2;
	random_sequence[28].led_pattern_i = 4;
	random_sequence[29].led_pattern_i = 3;
	random_sequence[30].led_pattern_i = 0;
	random_sequence[31].led_pattern_i = 5;
}

// LED Animation zu Beginn des Spiels
static void start_Game(void){

}

// Schreibt ein neues Level in sequrnce und gibt die Länge des Levels zurück
static uint8_t create_Level(uint8_t level, led_pattern *sequence){
	uint8_t length = sqrt(level) + 1;
	
	// zufälliges Auswählen von LEDs um das Level zu erstellen
	for(uint8_t i = 0;i<length;++i){
		sequence[i].led_pattern_i = (random(level,i,length));
	}
	return length;
}

// Setzt das Level in sequence in ein Blink-Muster um
static void blink_Led(led_pattern * const sequence, uint8_t length){

	sb_led_setMask(0xFF);
	sb_timer_delay(700);
	sb_led_setMask(0);
	sb_timer_delay(500);

	// Je nach Level bleiben die LEDs immer kürzer sichtbar
	for(uint8_t i=0;i<length;++i){
		uint8_t mask = (1<<sequence[i].led_pattern_i);
		sb_led_setMask(mask);
		sb_timer_delay(1000-50*length);
		sb_led_setMask(0);
		sb_timer_delay(700-50*length);
	}
}

// Warten auf Eingabe durch den Benutzer, hier wird das Potentiometer verwendbar gemacht
static uint8_t await_User(led_pattern *sequence, uint8_t length){
	uint8_t user_led_pattern = 0;

	// Iteration über das gesamte Level
	for(uint8_t i=0;i<length;i++){

		event = 0;
		
		// LED Steuerung mittls POTI
		while(!event){
			user_led_pattern = (sb_adc_read(POTI)/128);
			sb_led_setMask(1<<user_led_pattern);
		}
		
		// Prüft, ob richtige LED gewählt wurde
		if(user_led_pattern!=sequence[i].led_pattern_i){
		sb_led_setMask(0);
			return 1;
		}
	}
	return 0;
}

// Animation am Ende des Spiels
static void end_Game(uint8_t level, uint8_t error){
	level=level;
	if(error){

	}
	else{

	}
}


// Main
void main(void){

	init();
	sei();
	event = 0;
	show_Msg();

	while(1){

		start_Game();
		uint8_t level = 0;
		uint8_t error = 0;
		uint8_t length = 0;
		led_pattern sequence[10];

		while(level<0xFF&&!(error)){

			level = level + 1;
			sb_7seg_showHexNumber(level);
			sb_timer_delay(300);
			length = create_Level(level, sequence);
			blink_Led(sequence,length);
			sb_timer_delay(200);
			error = await_User(sequence,length);

		}

		end_Game(level,error);

	}
	cli();
}


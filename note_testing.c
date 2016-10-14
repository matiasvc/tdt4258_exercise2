#include <stdlib.h>

typedef enum
{
	 E2, //82.4
	 A2, //110.0
	 D3, //146.8
	 G3, //196.0
	 B3, //246.9
   E4 //329.6
} GuitarNotes;

AudioEffect GetFrequency(GuitarNotes gnote, float duration)
{
  float frequency;

  switch (gnote)
	{
		case E2 : { frequency = 82; } break;
		case A2 : { frequency = 110; } break;
		case D3 : { frequency = 147; } break;
		case G3 : { frequency = 196; } break;
		case B3 : { frequency = 247; } break;
    case E4 : { frequency = 330; } break;
	}

  AudioEffect effect = {};
	effect.type = SQUARE;
	effect.attackTime = 0.0;
	effect.sustainTime = duration;
	effect.decayTime = 0.0;
	effect.attackFrequency = 0;
	effect.sustainFrequency = frequency;
	effect.decayFrequency = 0;
	effect.frequencyAttackTransition = EASE_IN;
	effect.frequencyDecayTransition = EASE_OUT;
}

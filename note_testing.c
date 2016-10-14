#include <stdlib.h>

#include "efm32gg.h"
#include "audioMixer.h"

typedef enum
{
	 E2, //82.4
	 A2, //110.0
	 D3, //146.8
	 G3, //196.0
	 B3, //246.9
   E4 //329.6
} Notes;

typedef struct
{
  float from;
  float to;
} DurationTuple;

typedef struct
{
  AudioEffect* audioEffectPointers[6];
} AudioEffectPoiters;

typedef struct
{
  AudioEffect AudioEffects[50];
  DurationTuple Durations[50];
  uint16_t NoteCount;
} Melody;

uint16_t melody_initialize(Melody *melody, uint16_t noteCount);
AudioEffectPoiters melody_getRange(Melody *melody, float from, float to);
AudioEffect GetFrequency(Notes note, float duration);

uint16_t melody_initialize(Melody *melody, uint16_t noteCount)
{
  melody->NoteCount = noteCount;
  return noteCount;
}

void melody_addNote(Melody *melody, Notes note, float duration, float from, float to, uint16_t position)
{
  AudioEffect audioeffect = GetFrequency(note, duration);
  melody->AudioEffects[position] = audioeffect;
  melody->Durations[position] = (DurationTuple) { .from = from, .to = to};
}

// returnerer liste med pointers med audioeffects, null-terminert
AudioEffectPoiters melody_getRange(Melody *melody, float from, float to)
{
  AudioEffectPoiters audioEffectPointers;
  uint8_t arrayCount = 0;

  for(int i = 0; i < melody->NoteCount; i++)
  {
    if ((melody->Durations[i].from >= from && melody->Durations[i].from <= to)
      ||(melody->Durations[i].to >= from && melody->Durations[i].to <= to))
    {
      audioEffectPointers.audioEffectPointers[arrayCount++] = &(melody->AudioEffects[i]);
    }
  }

  if (arrayCount > 5) { arrayCount = 5; }
  audioEffectPointers.audioEffectPointers[arrayCount] = '\0';

  return audioEffectPointers;
}

AudioEffect GetFrequency(Notes note, float duration)
{
  float frequency;

  switch (note)
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

  return effect;
}

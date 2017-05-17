#include "stm32f10x.h"
#include "types.h"
#include "ToyCarManager.h"
#include "ToyCarOffState.h"
#include "ToyCarAvoidenceState.h"

void ToyCarFactory_CreateGameState( void )
{
    ToyCarManager_Init();
    OffState_Init();
    AvoidenceState_Init();
}


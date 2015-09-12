#include "BankerOwnedStates.h"
#include "fsm/State.h"
#include "Banker.h"
#include "Locations.h"
#include "messaging/Telegram.h"
#include "MessageDispatcher.h"
#include "MessageTypes.h"
#include "Time/CrudeTimer.h"
#include "EntityNames.h"

#include <iostream>
using std::cout;


#ifdef TEXTOUTPUT
#include <fstream>
extern std::ofstream os;
#define cout os
#endif


//------------------------------------------------------------------------methods for EnterBankAndWork
EnterBankAndWork* EnterBankAndWork::Instance()
{
  static EnterBankAndWork instance;

  return &instance;
}


void EnterBankAndWork::Enter(Banker* pBanker)
{
  //if the banker is not already located at the bank, he must
  //change location to the bank
  if (pBanker->Location() != bank)
  {
    cout << "\n" << GetNameOfEntity(pBanker->ID()) << ": " << "On my way to the bank!";

    pBanker->ChangeLocation(bank);
  }
}


void EnterBankAndWork::Execute(Banker* pBanker)
{  
  //Now the banker is at the bank and he works. If he gets thirsty
  // during his working he packs up work for a while and changes state to
  //go to the saloon for a whiskey.

  pBanker->IncreaseFatigue();

  cout << "\n" << GetNameOfEntity(pBanker->ID()) << ": " << "I'm working really really hard";

  if (pBanker->Thirsty())
  {
    pBanker->GetFSM()->ChangeState(QuenchThirstBanker::Instance());
  }
}


void EnterBankAndWork::Exit(Banker* pBanker)
{
  cout << "\n" << GetNameOfEntity(pBanker->ID()) << ": " 
       << "Ah'm leavin' the bank";
}


bool EnterBankAndWork::OnMessage(Banker* pBanker, const Telegram& msg)
{
  //send msg to global message handler
  return false;
}


//------------------------------------------------------------------------methods for GoHomeBankerAndSleepTilRested

GoHomeBankerAndSleepTilRested* GoHomeBankerAndSleepTilRested::Instance()
{
  static GoHomeBankerAndSleepTilRested instance;

  return &instance;
}

void GoHomeBankerAndSleepTilRested::Enter(Banker* pBanker)
{
  if (pBanker->Location() != shack_banker)
  {
    cout << "\n" << GetNameOfEntity(pBanker->ID()) << ": " << "Let's go home";

    pBanker->ChangeLocation(shack_banker); 
  }
}

void GoHomeBankerAndSleepTilRested::Execute(Banker* pBanker)
{ 
  //if banker is not fatigued start to work again.
  if (!pBanker->Fatigued())
  {
     cout << "\n" << GetNameOfEntity(pBanker->ID()) << ": " 
          << "I'm finally rested, let's work some more!";

     pBanker->GetFSM()->ChangeState(EnterBankAndWork::Instance());
  }

  else 
  {
    //sleep
    pBanker->DecreaseFatigue();

    cout << "\n" << GetNameOfEntity(pBanker->ID()) << ": " << "ZZZZZZZZZZZZZZZZZZZZZZZZZ... ";
  } 
}

void GoHomeBankerAndSleepTilRested::Exit(Banker* pBanker)
{ 
}


bool GoHomeBankerAndSleepTilRested::OnMessage(Banker* pBanker, const Telegram& msg)
{
  /*SetTextColor(BACKGROUND_RED|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);

   switch(msg.Msg)
   {
   case Msg_StewReady:

     cout << "\nMessage handled by " << GetNameOfEntity(pMiner->ID()) 
     << " at time: " << Clock->GetCurrentTime();

     SetTextColor(FOREGROUND_RED|FOREGROUND_INTENSITY);

     cout << "\n" << GetNameOfEntity(pMiner->ID()) 
          << ": Okay Hun, ahm a comin'!";

     pMiner->GetFSM()->ChangeState(EatStew::Instance());
      
     return true;

   }//end switch*/
   
   return false; //send message to global message handler
}

//------------------------------------------------------------------------QuenchThirstBanker

QuenchThirstBanker* QuenchThirstBanker::Instance()
{
  static QuenchThirstBanker instance;

  return &instance;
}

void QuenchThirstBanker::Enter(Banker* pBanker)
{
  if (pBanker->Location() != saloon)
  {    
    pBanker->ChangeLocation(saloon);

    cout << "\n" << GetNameOfEntity(pBanker->ID()) << ": " << "I need to drink something";
  }
}

void QuenchThirstBanker::Execute(Banker* pBanker)
{
  pBanker->BuyAndDrinkAWhiskey();

  cout << "\n" << GetNameOfEntity(pBanker->ID()) << ": " << "Pretty good taste for a whiskey";

  pBanker->GetFSM()->ChangeState(EnterBankAndWork::Instance());  
}


void QuenchThirstBanker::Exit(Banker* pBanker)
{ 
  cout << "\n" << GetNameOfEntity(pBanker->ID()) << ": " << "That sure is the best saloon! Let's return to the bank";
}


bool QuenchThirstBanker::OnMessage(Banker* pBanker, const Telegram& msg)
{
  //send msg to global message handler
  return false;
}



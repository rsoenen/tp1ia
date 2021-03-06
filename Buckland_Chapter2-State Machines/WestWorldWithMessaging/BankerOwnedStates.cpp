#include "BankerOwnedStates.h"
#include "fsm/State.h"
#include "Banker.h"
#include "Locations.h"
#include "messaging/Telegram.h"
#include "MessageDispatcher.h"
#include "MessageTypes.h"

#include "EntityNames.h"
#include <iostream>
using std::cout;


#ifdef TEXTOUTPUT
#include <fstream>
extern std::ofstream os;
#define cout os
#endif

bool bankerfight=false;

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
	pBanker->shared_print(pBanker->ID(), ": On my way to the bank!");
	pBanker->ChangeLocation(bank);

  }
}


void EnterBankAndWork::Execute(Banker* pBanker)
{  
  //Now the banker is at the bank and he works. If he gets thirsty
  // during his working he packs up work for a while and changes state to
  //go to the saloon for a whiskey.

  pBanker->IncreaseFatigue();

  pBanker->shared_print(pBanker->ID(), ": I'm working really really hard");
  if (pBanker->Thirsty())
  {
    pBanker->GetFSM()->ChangeState(QuenchThirstBanker::Instance());
  } else if (pBanker -> Fatigued())
  {
	pBanker->GetFSM()->ChangeState(GoHomeBankerAndSleepTilRested::Instance());
  }
}


void EnterBankAndWork::Exit(Banker* pBanker)
{
   pBanker->shared_print(pBanker->ID(), ": Ah'm leavin' the bank");
}


bool EnterBankAndWork::OnMessage(Banker* pBanker, const Telegram& msg)
{
 switch(msg.Msg)
  {
  case Msg_ImatBank:
   {
     pBanker ->shared_printTelegram(pBanker->ID());

	 pBanker->shared_print(pBanker->ID(), ": Ok Miner, I'm ready to collect ya nuggets!");
	 return true;
   }
 }
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
	pBanker->shared_print(pBanker->ID(), ": Let's go home");
    pBanker->ChangeLocation(shack_banker); 
  }
}

void GoHomeBankerAndSleepTilRested::Execute(Banker* pBanker)
{ 
  //if banker is not fatigued start to work again.
  //if (!pBanker->Fatigued()) SI ON FAIT COMME POUR MINEUR, IL NE PERD QUE UN POINT DE FATIGUE
  if (pBanker->ShowFatigue() == 0)
  {
	 pBanker->shared_print(pBanker->ID(), ": I'm finally rested, let's work some more!");
     pBanker->GetFSM()->ChangeState(EnterBankAndWork::Instance());
  }

  else 
  {
    //sleep
    pBanker->DecreaseFatigue();
	pBanker->shared_print(pBanker->ID(), ": ZZZZZZZZZZZZZZZZZZZZZZZZZ...");
  } 
}

void GoHomeBankerAndSleepTilRested::Exit(Banker* pBanker)
{ 
}


bool GoHomeBankerAndSleepTilRested::OnMessage(Banker* pBanker, const Telegram& msg)
{
  switch(msg.Msg)
  {
  case Msg_ImatBank:
   {
 
	 pBanker->shared_printTelegram(pBanker->ID());
  
	 pBanker->shared_print(pBanker->ID(), ": Ok Miner, I'm ready to collect ya nuggets!");
    
     pBanker->GetFSM()->ChangeState(EnterBankAndWork::Instance());
     return true;
   }
 }
   
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

	pBanker->shared_print(pBanker->ID(), ": Please Barman, give me some of your nicest liquer!");
	Dispatch->DispatchMessage(SEND_MSG_IMMEDIATELY, //time delay
                              pBanker->ID(),        //ID of sender
                              ent_Roger,            //ID of recipient
                              Msg_ImThirsty,   //the message
                              NO_ADDITIONAL_INFO);  
	Dispatch->DispatchMessage(SEND_MSG_IMMEDIATELY, //time delay
                              pBanker->ID(),        //ID of sender
                              ent_Miner_Bob,            //ID of recipient
                              Msg_ImThirsty,   //the message
                              NO_ADDITIONAL_INFO);    
	 
  }
}

void QuenchThirstBanker::Execute(Banker* pBanker)
{
  pBanker->BuyAndDrinkAWhiskey();
  pBanker->shared_print(pBanker->ID(), ": Pretty good taste for a whiskey");
  
  if (pBanker -> Fatigued()){
	  pBanker->GetFSM()->ChangeState(GoHomeBankerAndSleepTilRested::Instance());
  } else {
	  pBanker->GetFSM()->ChangeState(EnterBankAndWork::Instance()); 
  }
   
}


void QuenchThirstBanker::Exit(Banker* pBanker)
{ 
	if(!bankerfight)
	{

	if (pBanker -> Fatigued()){
		 pBanker->shared_print(pBanker->ID(), ": I'm too tired for returing work. Let's back home!");
	} else {
		 pBanker->shared_print(pBanker->ID(), ": That sure is the best saloon! Let's return to the bank");
	}
	}
	else
		bankerfight=false;
}



bool QuenchThirstBanker::OnMessage(Banker* pBanker, const Telegram& msg)
{
  switch(msg.Msg)
  {
  case Msg_ImatBank:
   {
      pBanker->shared_printTelegram(pBanker->ID());



	 pBanker->shared_print(pBanker->ID(), ": Ok Miner, I'm ready to collect ya nuggets!");
		pBanker->GetFSM()->ChangeState(EnterBankAndWork::Instance());
		return true;
   }
  case  Msg_ImThirsty:
   {
	   bankerfight=true;
     
	   pBanker->shared_print(pBanker->ID(), ": Ok Miner, I'm ready to fight with you");
	   pBanker->shared_print(pBanker->ID(), ": You shouldn't test me, Miner!");

	   pBanker->GetFSM()->ChangeState(BankerFighting::Instance());
	 return true;
   }
 }
  return false;
}


// Now the banker wants to fight with the miner

BankerFighting* BankerFighting::Instance()
{
  static BankerFighting instance;
  
  return &instance;
}

void BankerFighting::Enter(Banker* pBanker)
{
	Dispatch->DispatchMessage(SEND_MSG_IMMEDIATELY, //time delay
                              pBanker->ID(),        //ID of sender
                              ent_Miner_Bob,            //ID of recipient
                              Msg_ImThirsty,   //the message
                              NO_ADDITIONAL_INFO);  
}

void BankerFighting::Execute(Banker* pBanker)
{
	
  pBanker->BuyAndDrinkAWhiskey();

	
  Dispatch->DispatchMessage(SEND_MSG_IMMEDIATELY, //time delay
                              pBanker->ID(),        //ID of sender
                              ent_Roger,            //ID of recipient
                              Msg_IWannaFight,   //the message
                              NO_ADDITIONAL_INFO);  
  
  if (pBanker -> Fatigued()){
	  pBanker->GetFSM()->ChangeState(GoHomeBankerAndSleepTilRested::Instance());
	  
  } else {
	  pBanker->GetFSM()->ChangeState(EnterBankAndWork::Instance()); 
	  
  }
   
}


void BankerFighting::Exit(Banker* pBanker)
{ 
	

	pBanker->shared_print(pBanker->ID(), ": Sorry for that Roger, I promise you it won't happen again");
}



bool BankerFighting::OnMessage(Banker* pBanker, const Telegram& msg)
{
	return false;
}


BankerOwnedStates::BankerOwnedStates(void)
{
}


BankerOwnedStates::~BankerOwnedStates(void)
{
}

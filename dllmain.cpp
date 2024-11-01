#include <stdio.h>
#include <bit>
#include <string>
#include <thread>
#include <chrono>

#include <Mod/CppUserModBase.hpp>
#include <DynamicOutput/DynamicOutput.hpp>
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/UObject.hpp>
#include <Unreal/FText.hpp>
#include <Unreal/FProperty.hpp>
#include <Unreal/UFunction.hpp>
#include <Unreal/BPMacros.hpp>
#include <UE4SSProgram.hpp>
#include <Unreal/UClass.hpp>

using namespace std;
using namespace RC;
using namespace RC::Unreal;



struct InputParams {
    UObject* WaveManager;
} in;


void HookPre_WaveTimer_GetWaveTimes(UnrealScriptFunctionCallableContext& Context, void* CustomData)
{
    auto params = Context.GetParams<InputParams>();
    //Output::send<LogLevel::Verbose>(STR("Object Name: {}\n"), params.WaveManager->GetName());

    float* arr = (float*)((void*)(params.WaveManager + 0x134));
    Output::send<LogLevel::Verbose>(STR("Normal Wave: {} Sripted Wave: {}\n"), (int)arr[0], (int)arr[1]);
    //Output::send<LogLevel::Verbose>(STR("Object Name: {}\n"), Context.Context->GetName());

    struct {
        int NormalWave;
        int SriptedWave;
    }params1;

    params1.NormalWave = (int)arr[1];
    params1.SriptedWave = (int)arr[0];
    auto SetWaveTimesFunction = UObjectGlobals::StaticFindObject<UFunction*>(nullptr, nullptr, STR("/Game/EnemyWaveTimer/WaveTimer.WaveTimer_C:SetWaveTimes"));
    //Output::send<LogLevel::Verbose>(STR("Object Name: {}\n"), SetWaveTimesFunction->GetName());
    Context.Context->ProcessEvent(SetWaveTimesFunction, &params1);
}

void HookPost_WaveTimer_GetWaveTimes(UnrealScriptFunctionCallableContext& Context, void* CustomData)
{
}


bool isHooked = false;
void OnCreate(){
        if (isHooked){
            return;
        }
        // You are allowed to use the 'Unreal' namespace in this function and anywhere else after this function has fired.
        auto GetWaveTimesFunction = UObjectGlobals::StaticFindObject<UFunction*>(nullptr, nullptr, STR("/Game/EnemyWaveTimer/WaveTimer.WaveTimer_C:GetWaveTimes"));
        Output::send<LogLevel::Verbose>(STR("Object Name: {}\n"), GetWaveTimesFunction->GetFullName());
        if (GetWaveTimesFunction){
            UObjectGlobals::RegisterHook(GetWaveTimesFunction, HookPre_WaveTimer_GetWaveTimes, HookPost_WaveTimer_GetWaveTimes, nullptr);
            Output::send<LogLevel::Verbose>(STR("Hook success"));
            isHooked = true;
        }
}


class MyAwesomeMod : public RC::CppUserModBase
{
public:
    MyAwesomeMod() : CppUserModBase()
    {
        ModName = STR("WaveTimer");
        ModVersion = STR("1.0");
        ModDescription = STR("This is my awesome mod");
        ModAuthors = STR("iriscat");
        // Do not change this unless you want to target a UE4SS version
        // other than the one you're currently building with somehow.
        //ModIntendedSDKVersion = STR("2.6");

    }

    ~MyAwesomeMod() override
    {
    }

    auto on_update() -> void override
    {
        OnCreate();
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    auto on_unreal_init() -> void override
    {
        
        // You are allowed to use the 'Unreal' namespace in this function and anywhere else after this function has fired.
    }

    auto on_program_start() -> void override
    {
    }

};

#define MY_AWESOME_MOD_API __declspec(dllexport)
extern "C"
{
    MY_AWESOME_MOD_API RC::CppUserModBase* start_mod()
    {
        return new MyAwesomeMod();
    }

    MY_AWESOME_MOD_API void uninstall_mod(RC::CppUserModBase* mod)
    {
        delete mod;
    }
}
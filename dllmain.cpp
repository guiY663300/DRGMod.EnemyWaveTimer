#include <stdio.h>
#include <bit>
#include <string>
#include <thread>
#include <chrono>

#include <Mod/CppUserModBase.hpp>
#include <DynamicOutput/DynamicOutput.hpp>
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/UObject.hpp>
#include <Unreal/UFunction.hpp>
#include <Unreal/BPMacros.hpp>
#include <UE4SSProgram.hpp>

using namespace std;
using namespace RC;
using namespace RC::Unreal;


struct InputParams {
    UObject* WaveManager;
} in;

int NormalWave = 0;
int SriptedWave = 0;

void HookPre_WaveTimer_GetWaveTimes(UnrealScriptFunctionCallableContext& context, void* custom_data)
{
    auto params = context.GetParams<InputParams>();

    float* arr = (float*)((void*)(params.WaveManager + 0x134));
    //Output::send<LogLevel::Verbose>(STR("Normal Wave: {} Sripted Wave: {}\n"), (int)arr[0], (int)arr[1]);

    NormalWave = (int)arr[1];
    SriptedWave = (int)arr[0];
}

void HookPost_WaveTimer_GetWaveTimes(UnrealScriptFunctionCallableContext& context, void* custom_data)
{
    auto param = context.TheStack.OutParms();
    *param->PropAddr = NormalWave;

    param = param->NextOutParm;
    *param->PropAddr = SriptedWave;
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
        ModVersion = STR("1.1");
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
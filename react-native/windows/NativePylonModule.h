// Windows-Adapter (C++/WinRT, react-native-windows, Fabric/TurboModule).
// Registrierung: IReactPackageBuilder::AddTurboModule statt AddModule
// (siehe RNW-Doku "Native Modules vs TurboModules").
// Reine JSI-Marshalling-Schicht — Geraete-/Grab-Logik liegt in pylon_core_*.
#pragma once
// Codegen-Scaffold (wird von `codegen` erzeugt, hier als Include referenziert):
//   #include <AppSpecsJSI.h>
// #include "../shared/pylon_core.h"
// class NativePylonModule : public facebook::react::NativePylonCxxSpec<NativePylonModule> {
//  public:
//   NativePylonModule(std::shared_ptr<facebook::react::CallInvoker> jsInvoker);
//   std::string getVersion(facebook::jsi::Runtime& rt);
//   std::string getPylonVersion(facebook::jsi::Runtime& rt);
//   bool isMock(facebook::jsi::Runtime& rt);
//   // ... enumerateDevices / grabMock delegieren an node_pylon::core::*
// };
//
// Registrierung im PackageProvider (Skizze):
//   void CreatePackage(IReactPackageBuilder const& packageBuilder) noexcept {
//     AddAttributedModules(packageBuilder, true); // true = TurboModules
//   }

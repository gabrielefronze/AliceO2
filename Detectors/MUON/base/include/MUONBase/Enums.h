#ifndef O2_DEV_ALO_ENUMS_H
#define O2_DEV_ALO_ENUMS_H

///
///  @file   Enums
///  @author Gabriele G. Fronzé <gfronze at cern.ch>
///  @date   24 July 2017
///  @brief  Conatiner for usefull enumerators
///

namespace AliceO2 {

    namespace MUON {

        enum digitType {
            kPhysics,
            kFET,
            kTriggered,
            kSize
        };


        typedef enum {
            kShortMsg,
            kFailedSend,
            kOk
        } errMsg;

    }
}

#endif //O2_DEV_ALO_ENUMS_H

//
// Created by Gabriele Gaetano Fronzé on 06/07/2017.
//

#ifndef O2_DEV_ALO_ENUMS_H
#define O2_DEV_ALO_ENUMS_H

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

//
// Copyright (c) 2020 bismark LLC All rights reserved
//

package jp.bismark.bssynth

class Instrument constructor(val bankMsb: Int, val bankLsb: Int, val program: Int, val name: String) {

    fun info(): String {
        return "$name ($bankMsb/$bankLsb/$program)"
    }

}

//
// Copyright (c) 2020 bismark LLC All rights reserved
//

package jp.bismark.sample

import android.R
import android.content.Context
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ArrayAdapter
import android.widget.TextView
import jp.bismark.bssynth.Instrument


class InstrumentAdapter(context: Context, objects: List<Instrument?>?) :
    ArrayAdapter<Instrument?>(context, 0, objects!!) {
    private val mLayoutInflater = LayoutInflater.from(context)

    override fun getView(position: Int, convertView: View?, parent: ViewGroup): View {
        val view = convertView ?: mLayoutInflater.inflate(R.layout.simple_list_item_single_choice, parent, false)
        val instrument = getItem(position)
        instrument?.let {
            val textView = view.findViewById(R.id.text1) as TextView
            textView.text = it.info()
        }
        return view
    }
}


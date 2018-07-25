#!/bin/bash

custardctl --configure debug_mode false

custardctl --configure grid_rows 2
custardctl --configure grid_columns 3

custardctl --configure grid_gap 0
custardctl --configure grid_margin_top 0
custardctl --configure grid_margin_bottom 0
custardctl --configure grid_margin_left 0
custardctl --configure grid_margin_right 0

custardctl --configure border_focused_color \#ffffffff
custardctl --configure border_unfocused_color \#676767ff
custardctl --configure border_background_color \#000000ff

custardctl --configure border_inner_size 0
custardctl --configure border_outer_size 0
custardctl --configure border_type 0

custardctl --configure groups 2

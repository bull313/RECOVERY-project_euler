from datetime import timedelta
from sys import stdout
from time import time

start_time = None
start_value = None

def progress_bar(current_value, target_value, bar_length=20, loading_bar_fill_char='#', loading_bar_empty_char=' ', include_time_estimate=False):
    percent_progress = float(current_value) / target_value

    global start_time
    global start_value
    end_time = None
    end_value = None
    estimated_completion = None
    if not start_time and not start_value:
        start_time = time()
        start_value = percent_progress
    else:
        end_value = percent_progress
        elapsed_value = (end_value - start_value) * 100.0

        if elapsed_value >= 1.0:
            end_time = time()

    if start_time and end_time:
        elapsed_time = end_time - start_time
        estimated_completion = elapsed_time * (100.0 - percent_progress * 100.0)
        start_time = None
        start_value = None

    est_complete_str = None
    if estimated_completion:
        est_complete_str = "Estimated time remaining: %s" % str(timedelta(seconds=estimated_completion))

    fill_size = int(round(percent_progress * bar_length) - 1)
    loading_bar_filled = loading_bar_fill_char * fill_size
    loading_bar_empty = loading_bar_empty_char * (bar_length - fill_size)
    loading_bar = loading_bar_filled + loading_bar_empty

    if not est_complete_str or not include_time_estimate:
        stdout.write("\rHang on, we're LOADING!!! [%s] %.2f%% complete." % (loading_bar, percent_progress * 100.0))
    else:
        stdout.write("\rHang on, we're LOADING!!! [%s] %.2f%% complete. %s" % (loading_bar, percent_progress * 100.0, est_complete_str))

    stdout.flush()

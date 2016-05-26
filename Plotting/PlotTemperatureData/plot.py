#!/usr/bin/env python

"""
Based on matplotlib date_demo.py API example.
Read at: http://matplotlib.org/examples/api/date_demo.html
"""

import datetime
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import json, urllib
import constants

years = mdates.YearLocator()   # every year
months = mdates.MonthLocator()  # every month
days = mdates.DayLocator() # every day
hours = mdates.HourLocator() # every hour
yearsFmt = mdates.DateFormatter('%Y')
monthsFmt = mdates.DateFormatter('%m')
daysFmt = mdates.DateFormatter('%d/%m')
#hoursFmt = mdates.DateFormatter('%H')

print "Data fetching from server beginning"
data = urllib.urlopen(constants.LOG_SERVER_BASE_URL + "/get_data.php").read()
print "Data fetching from server done"
output = json.loads(data)
output = [row for row in output if row["logger_name"] in constants.DEFAULT_LOGGER_NAMES_FILTER]

fig, ax = plt.subplots()
temperatures = [float(row["temperature_celsius"]) for row in output]
humidities = [float(row["humidity"]) for row in output]
heat_indexes = [float(row["heat_index_celsius"]) for row in output]
times = [datetime.datetime.utcfromtimestamp(float(row["log_time"])) for row in output]
#lines = ax.plot(times, temperatures)
#lines = ax.plot(times, heat_indexes)
#lines = ax.plot(times, temperatures, times, heat_indexes)
lines = ax.plot(times, temperatures, times, humidities)
plt.setp(lines, marker=".")

# format the ticks
ax.xaxis.set_major_locator(days)
ax.xaxis.set_major_formatter(daysFmt)
ax.xaxis.set_minor_locator(hours)
#ax.xaxis.set_minor_formatter(hoursFmt)
ax.xaxis.set_tick_params(which='minor', pad=15)


datemin = min(times)
datemax = max(times)
ax.set_xlim(datemin, datemax)


# format the coords message box
def price(x):
    return '$%1.2f' % x
ax.format_xdata = mdates.DateFormatter('%Y-%m-%d')
ax.format_ydata = price
ax.grid(True)

# rotates and right aligns the x labels, and moves the bottom of the
# axes up to make room for them
fig.autofmt_xdate()


plt.show()

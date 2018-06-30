# logToCSV

logToCSV converts log files into CSV files.

Options are available to select one or all channels as well as filtering one specific
transmitter ID and one specific resolution.

```
Usage of logToCSV:
  -c int
        Optional Channel Index. All channels if -1. (default -1)
  -in string
        Log file in JSON format. (default "log.json")
  -out string
        Output file name (default "log.csv")
  -r int
        Optional Resolution. All resolutions if -1. (default -1)
  -sep
        Set to create multiple files containing one tId each.
  -tId int
        Optional Transmitter ID. All transmitters if -1. (default -1)
```
package main

import (
	"encoding/json"
	"flag"
	"io/ioutil"
	"log"
	"strconv"
	"strings"
)

var inputFile string
var outputFile string
var transmitterId int64
var channelIndex int
var resolutionFilter int64
var separateTransmitterIds bool

func main() {

	flag.StringVar(&inputFile, "in", "log.json", "Log file in JSON format.")
	flag.StringVar(&outputFile, "out", "log.csv", "Output file name")
	flag.Int64Var(&transmitterId, "tId", -1, "Optional Transmitter ID. All transmitters if -1.")
	flag.IntVar(&channelIndex, "c", -1, "Optional Channel Index. All channels if -1.")
	flag.Int64Var(&resolutionFilter, "r", -1, "Optional Resolution. All resolutions if -1.")
	flag.BoolVar(&separateTransmitterIds, "sep", false, "Set to create multiple files containing one tId each.")

	flag.Parse()

	outputFile = strings.TrimSuffix(outputFile, ".csv")

	b, err := ioutil.ReadFile(inputFile)
	if err != nil {
		log.Fatal(err)
	}

	var flightLog Log
	err = json.Unmarshal(b, &flightLog)
	if err != nil {
		log.Fatal(err)
	}

	if separateTransmitterIds {
		writeMultipleCSV(flightLog)
	} else {
		writeSingleCSV(flightLog)
	}

}

func writeMultipleCSV(flightLog Log) {
	csvData := make(map[int64][]byte)
	for _, p := range flightLog.Recording {
		if transmitterId == -1 || p.Data.Header.TransmitterID == transmitterId {
			if resolutionFilter == -1 || p.Data.Header.Resolution == resolutionFilter {

				var data string
				var cMax int
				if channelIndex == -1 {
					cMax = len(p.Data.Channels)
				} else {
					cMax = channelIndex
				}
				for c := 0; c < cMax; c++ {
					data += strconv.FormatInt(p.Data.Channels[c], 10) + ";"
				}
				csvData[p.Data.Header.TransmitterID] = append(csvData[p.Data.Header.TransmitterID], []byte(strconv.FormatInt(p.Timestamp, 10)+";"+strconv.FormatInt(p.Data.Header.TransmitterID, 10)+";"+data[0:len(data)-1]+"\n")...)
			}
		}
	}
	for tid, data := range csvData {
		err := ioutil.WriteFile(outputFile+"_"+strconv.FormatInt(tid, 10)+".csv", data, 0666)
		if err != nil {
			log.Fatal(err)
		}
	}
}

func writeSingleCSV(flightLog Log) {
	var csvData []byte
	for _, p := range flightLog.Recording {
		if transmitterId == -1 || p.Data.Header.TransmitterID == transmitterId {
			if resolutionFilter == -1 || p.Data.Header.Resolution == resolutionFilter {

				var data string
				var cMax int
				if channelIndex == -1 {
					cMax = len(p.Data.Channels)
				} else {
					cMax = channelIndex
				}
				for c := 0; c < cMax; c++ {
					data += strconv.FormatInt(p.Data.Channels[c], 10) + ";"
				}
				csvData = append(csvData, []byte(strconv.FormatInt(p.Timestamp, 10)+";"+strconv.FormatInt(p.Data.Header.TransmitterID, 10)+";"+data[0:len(data)-1]+"\n")...)
			}
		}
	}
	err := ioutil.WriteFile(outputFile, csvData, 0666)
	if err != nil {
		log.Fatal(err)
	}
}

type Log struct {
	Recording []Packet `json:"recording"`
	Starttime int64    `json:"starttime"`
}

type Packet struct {
	Data struct {
		Channels []int64 `json:"channels"`
		Header   Header  `json:"header"`
	} `json:"data"`
	Timestamp int64 `json:"timestamp"`
}

type Header struct {
	ChannelCount  int64 `json:"channelCount"`
	Resolution    int64 `json:"resolution"`
	TransmitterID int64 `json:"transmitterId"`
}

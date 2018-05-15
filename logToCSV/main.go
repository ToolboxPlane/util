package main

import (
	"encoding/json"
	"flag"
	"io/ioutil"
	"log"
	"strconv"
)

func main() {

	inputFile := flag.String("in", "log.json", "Log file in JSON format.")
	outputFile := flag.String("out", "log.csv", "Output file name")
	transmitterId := flag.Int64("tId", -1, "Optional Transmitter ID. All transmitters if -1.")
	channelIndex := flag.Int("c", -1, "Optional Channel Index. All channels if -1.")
	resolutionFilter := flag.Int64("r", -1, "Optional Resolution. All resolutions if -1.")
	flag.Parse()

	b, err := ioutil.ReadFile(*inputFile)
	if err != nil {
		log.Fatal(err)
	}

	var flightLog Log
	err = json.Unmarshal(b, &flightLog)
	if err != nil {
		log.Fatal(err)
	}

	var csvData []byte
	for _, p := range flightLog.Recording {
		if *transmitterId == -1 || p.Data.Header.TransmitterID == *transmitterId {
			if *resolutionFilter == -1 || p.Data.Header.Resolution == *resolutionFilter {

				var data string
				var cMax int
				if *channelIndex == -1 {
					cMax = len(p.Data.Channels)
				} else {
					cMax = *channelIndex
				}
				for c := 0; c < cMax; c++ {
					data += strconv.FormatInt(p.Data.Channels[c], 10) + ";"
				}
				csvData = append(csvData, []byte(strconv.FormatInt(p.Timestamp, 10)+";"+data[0:len(data)-1]+"\n")...)
			}
		}
	}
	err = ioutil.WriteFile(*outputFile, csvData, 0666)
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

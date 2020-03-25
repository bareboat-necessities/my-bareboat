package com.bareboat_necessities.ais;

import dk.tbsalling.aismessages.AISInputStreamReader;

import java.io.IOException;

public class Boot {
    public static void main(String[] args) throws IOException {
        var classLoader = Thread.currentThread().getContextClassLoader();
        try (var inputStream = classLoader.getResourceAsStream("nmea-ais-sample.txt")) {
            var streamReader = new AISInputStreamReader(inputStream, System.out::println);
            streamReader.run();
        }
    }
}
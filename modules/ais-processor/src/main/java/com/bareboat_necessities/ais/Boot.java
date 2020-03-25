package com.bareboat_necessities.ais;

import dk.tbsalling.aismessages.AISInputStreamReader;
import dk.tbsalling.aismessages.ais.messages.AISMessage;
import dk.tbsalling.aismessages.ais.messages.PositionReport;
import dk.tbsalling.aismessages.ais.messages.ShipAndVoyageData;

import java.io.IOException;

public class Boot {

    private AisCache cache = new AisCache();

    public static void main(String[] args) throws IOException {
        var boot = new Boot();
        boot.run();
    }

    public void run() throws IOException {
        var thread = new Thread(() -> {
            while (true) {
                var ships = cache.getShips();
                ships.asMap().forEach((mmsi, ship) -> {
                    var m = cache.getShipPositions().getIfPresent(ship.getSourceMmsi());
                    if (m != null) {
                        System.out.print(m.getSourceMmsi() + " " + m.getLatitude() + " " + m.getLongitude());
                        System.out.println(" " + ship.getShipName() + " type=" + ship.getShipType());
                    }
                });
                try {
                    Thread.sleep(1000L);
                } catch (InterruptedException e) {
                    throw new RuntimeException("InterruptedException", e);
                }
            }
        });
        //thread.setDaemon(true);
        thread.start();
        var classLoader = Thread.currentThread().getContextClassLoader();
        try (var inputStream = classLoader.getResourceAsStream("nmea-ais-sample.txt")) {
            var streamReader = new AISInputStreamReader(inputStream, this::process);
            streamReader.run();
        }
    }

    public void process(AISMessage msg) {
        if (msg instanceof ShipAndVoyageData) {
            var m = ((ShipAndVoyageData) msg);
            cache.getShips().put(m.getSourceMmsi(), m);
        } else if (msg instanceof PositionReport) {
            var m = ((PositionReport) msg);
            cache.getShipPositions().put(m.getSourceMmsi(), m);
        }
    }
}
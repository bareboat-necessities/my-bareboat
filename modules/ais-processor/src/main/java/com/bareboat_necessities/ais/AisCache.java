package com.bareboat_necessities.ais;

import com.google.common.cache.Cache;
import com.google.common.cache.CacheBuilder;

import dk.tbsalling.aismessages.ais.messages.PositionReport;
import dk.tbsalling.aismessages.ais.messages.ShipAndVoyageData;
import dk.tbsalling.aismessages.ais.messages.types.MMSI;

import java.time.Duration;

public class AisCache {
    private Cache<MMSI, ShipAndVoyageData> ships = CacheBuilder.newBuilder()
            .expireAfterWrite(Duration.ofMinutes(30)).build();

    private Cache<MMSI, PositionReport> shipPositions = CacheBuilder.newBuilder()
            .expireAfterWrite(Duration.ofMinutes(30)).build();

    public Cache<MMSI, ShipAndVoyageData> getShips() {
        return ships;
    }

    public Cache<MMSI, PositionReport> getShipPositions() {
        return shipPositions;
    }
}

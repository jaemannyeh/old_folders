--
--
--

PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;

CREATE TABLE IF NOT EXISTS DEVICE_DATA (
    NAME TEXT UNIQUE NOT NULL,
    VALUE TEXT,
    STATIC INTEGER DEFAULT 0,
    OPTIONAL INTEGER DEFAULT 0,
    DESCRIPTION TEXT DEFAULT ''
);

INSERT OR IGNORE INTO DEVICE_DATA VALUES('Device.DeviceInfo.Manufacturer','value',0,0,'');
INSERT OR IGNORE INTO DEVICE_DATA VALUES('Device.ManagementServer.URL','value',0,0,'');
INSERT OR IGNORE INTO DEVICE_DATA VALUES('Device.Config.ConfigFile','value',0,0,'');
INSERT OR IGNORE INTO DEVICE_DATA VALUES('Device.Time.CurrentLocalTime','value',0,0,'');
INSERT OR IGNORE INTO DEVICE_DATA VALUES('Device.Time.LocalTimeZone','EST+5 EDT,M4.1.0/2,M10.5.0/2',0,0,'');
INSERT OR IGNORE INTO DEVICE_DATA VALUES('Device.UserInterface.WarrantyDate','value',0,0,'');

COMMIT;

--
--
--

.databases
.tables

--
--
--

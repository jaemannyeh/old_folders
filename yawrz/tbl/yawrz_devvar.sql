--
--
--

PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;

CREATE TABLE IF NOT EXISTS DEVVAR (
    NAME TEXT UNIQUE NOT NULL,
    VALUE TEXT,
    STATIC INTEGER DEFAULT 0,
    OPTIONAL INTEGER DEFAULT 0,
    DESCRIPTION TEXT DEFAULT ''
);

INSERT OR IGNORE INTO DEVVAR VALUES('Device.DeviceInfo.DeviceCategory','0',0,0,'');
INSERT OR IGNORE INTO DEVVAR VALUES('Device.DeviceInfo.Manufacturer','0',0,0,'');
INSERT OR IGNORE INTO DEVVAR VALUES('Device.DeviceInfo.ManufacturerOUI','0',0,0,'');
INSERT OR IGNORE INTO DEVVAR VALUES('Device.DeviceInfo.ModelName','0',0,0,'');
INSERT OR IGNORE INTO DEVVAR VALUES('Device.DeviceInfo.ModelNumber','0',0,0,'');
INSERT OR IGNORE INTO DEVVAR VALUES('Device.DeviceInfo.Description','0',0,0,'');
INSERT OR IGNORE INTO DEVVAR VALUES('Device.DeviceInfo.ProductClass','0',0,0,'');
INSERT OR IGNORE INTO DEVVAR VALUES('Device.DeviceInfo.SerialNumber','0',0,0,'');
INSERT OR IGNORE INTO DEVVAR VALUES('Device.DeviceInfo.HardwareVersion','0',0,0,'');
INSERT OR IGNORE INTO DEVVAR VALUES('Device.DeviceInfo.SoftwareVersion','0',0,0,'');
INSERT OR IGNORE INTO DEVVAR VALUES('Device.DeviceInfo.AdditionalHardwareVersion','0',0,0,'');
INSERT OR IGNORE INTO DEVVAR VALUES('Device.DeviceInfo.AdditionalSoftwareVersion','0',0,0,'');
INSERT OR IGNORE INTO DEVVAR VALUES('Device.DeviceInfo.ProvisioningCode','0',0,0,'');
INSERT OR IGNORE INTO DEVVAR VALUES('Device.DeviceInfo.UpTime','0',0,0,'seconds');
INSERT OR IGNORE INTO DEVVAR VALUES('Device.DeviceInfo.FirstUseDate','0',0,0,'');
INSERT OR IGNORE INTO DEVVAR VALUES('Device.DeviceInfo.VendorConfigFileNumberOfEntries','0',0,0,'');
INSERT OR IGNORE INTO DEVVAR VALUES('Device.DeviceInfo.SupportedDataModelNumberOfEntries','0',0,0,'');
INSERT OR IGNORE INTO DEVVAR VALUES('Device.DeviceInfo.ProcessorNumberOfEntries','0',0,0,'');
INSERT OR IGNORE INTO DEVVAR VALUES('Device.DeviceInfo.VendorLogFileNumberOfEntries','0',0,0,'');
INSERT OR IGNORE INTO DEVVAR VALUES('Device.DeviceInfo.LocationNumberOfEntries','0',0,0,'');

--
--
--

COMMIT;
.databases
.tables

--
--
--

-- SELECT * FROM DEVVAR;
-- SELECT * FROM FAPSERVICE;
-- SELECT * FROM STORAGESERVICE;

--
--
--

use GENY_DATA;
DROP TABLE IF EXISTS OEILLERES;
CREATE TABLE OEILLERES
(
	ID INT,
    DESCR VARCHAR(20),
    PRIMARY KEY (ID)
)
ENGINE=InnoDB DEFAULT CHARSET=utf8;
CREATE UNIQUE INDEX ID ON OEILLERES (ID);

insert OEILLERES values (0, 'sans'), (1, 'australiennes'), (3, 'normales');


DROP TABLE IF EXISTS IRON_REMOVAL;
CREATE TABLE IRON_REMOVAL
(
	ID INT,
    DESCR VARCHAR(20),
    PRIMARY KEY (ID)
)
ENGINE=InnoDB DEFAULT CHARSET=utf8;
CREATE UNIQUE INDEX ID ON IRON_REMOVAL (ID);

insert IRON_REMOVAL values (0, 'sans'), (1, 'avant'), (2, 'arriere'), (3, 'total');


DROP TABLE IF EXISTS COTE_EVEOL;
CREATE TABLE COTE_EVEOL
(
	ID INT,
    DESCR VARCHAR(20),
    IMG BLOB NOT NULL,
    PRIMARY KEY (ID)
)
ENGINE=InnoDB DEFAULT CHARSET=utf8;
CREATE UNIQUE INDEX ID ON COTE_EVEOL (ID);

insert 
	COTE_EVEOL
values 
	(0, 'ras', LOAD_FILE('/home/farouk/web_robot/PICTOS/cote0.gif')), 
	(1, 'hausse', LOAD_FILE('/home/farouk/web_robot/PICTOS/cote1.gif')), 
    (10, 'hausse importante', LOAD_FILE('/home/farouk/web_robot/PICTOS/cote2.gif')), 
	(-1, 'baisse', LOAD_FILE('/home/farouk/web_robot/PICTOS/cote-1.gif')), 
    (-10, 'baisse importante', LOAD_FILE('/home/farouk/web_robot/PICTOS/cote-2.gif'));


-- DROP TABLE IF EXISTS CASAQUES;
-- CREATE TABLE CASAQUES
-- (
--     LNK VARCHAR(500),
--     IMG BLOB,
--     PRIMARY KEY (LNK)
-- )
-- ENGINE=InnoDB DEFAULT CHARSET=utf8;
-- CREATE UNIQUE INDEX ID ON CASAQUES (LNK);









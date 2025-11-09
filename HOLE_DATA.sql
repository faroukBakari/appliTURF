
SET GLOBAL connect_timeout=120000;

SET SESSION MAX_EXECUTION_TIME=120000;
SET GLOBAL MAX_EXECUTION_TIME=120000;

DROP TABLE IF EXISTS GENY_DATA.HOLE_DATA;
CREATE TABLE GENY_DATA.HOLE_DATA
(

	DT DATE,
	RACE_ID INT not null,
	H_DEPART TIME,
	NB_GREP INT,
	HYPPO VARCHAR(200),
	NB_RACE INT,
	R_NAME VARCHAR(200),
	TYPE_C VARCHAR(100),
	SPECIALITE VARCHAR(100),
	COURSE VARCHAR(100),
	CATEGORY VARCHAR(100),
	CORDE VARCHAR(100),
	PRIZE INT,
	R_LENGHT INT,
	TERRAIN VARCHAR(100),
	ET_GAZON FLOAT,
	PISTE VARCHAR(200),
	SPEC VARCHAR(200),
	STAT_LNK VARCHAR(500),
	HORSE VARCHAR(200),
	HORSE_ID INT not null,
	OEIL INT,
	OEIL_DESC VARCHAR(20),
	POS_DEPART INT,
	HR_LENGTH INT,
	IRON_REMOV INT,
	IRON_DESC VARCHAR(20),
	DECHARGE FLOAT,
	SEX  VARCHAR(10),
	AGE INT,
	WEIGHT FLOAT,
	DRIVER VARCHAR(200),
	DRIVER_ID INT,
	COACH VARCHAR(200),
	COACH_ID INT, 
	MUSIC VARCHAR(200),
	WINNING INT,
	VALEUR FLOAT,
	RANK INT,
	FAIL VARCHAR(10),
	UNIT_TIME FLOAT,
	VISUAL_GAP VARCHAR(200),
	COTE_INIT FLOAT,
	COTE_F FLOAT,
	EVOL INT,
	EVOL_DESC VARCHAR(200),
	EVOL_IMG BLOB,
	RATIO_INIT FLOAT,
	RATIO_F FLOAT,
	CASAQ_LNK VARCHAR(500),
	CASAQ_IMG BLOB,
    
    PRIMARY KEY (RACE_ID, HORSE_ID)
)
ENGINE=InnoDB DEFAULT CHARSET=utf8;
CREATE UNIQUE INDEX ID ON GENY_DATA.HOLE_DATA (RACE_ID, HORSE_ID);

INSERT INTO HOLE_DATA
	select
		summary.DT,
		summary.RACE_ID,
		summary.H_DEPART,
		summary.NB_GREP,
		summary.HYPPO,
		summary.NB_RACE,
		summary.R_NAME,
		summary.TYPE_C,
		summary.SPECIALITE,
		summary.COURSE,
		summary.categorie,
		summary.CORDE,
		summary.PRIZE,
		summary.R_LENGHT,
		summary.TERRAIN,
		summary.ET_GAZON,
		summary.PISTE,
		summary.SPEC,
        summary.STAT_LNK,
		rdata.HORSE,
		rdata.HORSE_ID,
		rdata.OEIL,
		oei.DESCR as OEIL_DESC,
		rdata.POS_DEPART,
		rdata.HR_LENGTH,
		rdata.IRON_REMOV,
		iron.DESCR as IRON_DESC,
		rdata.DECHARGE,
		rdata.SEX,
		rdata.AGE,
		rdata.WEIGHT,
		rdata.DRIVER,
		rdata.DRIVER_ID,
		rdata.COACH,
		rdata.COACH_ID, 
		rdata.MUSIC,
		rdata.WINNING,
		rdata.VALEUR,
		rslt.RANK,
		rslt.FAIL,
		rslt.UNIT_TIME,
		rslt.VISUAL_GAP,
		cotes.COTE_30mn,
		cotes.COTE_F,
		cotes.EVOL,
		pic.DESCR as EVOL_DESC,
		pic.IMG as EVOL_IMG,
		cotes.RATIO_30mn,
		cotes.RATIO_F,
		casaques.LNK,
		casaq.IMG as CASAQ_IMG
			
	from		
		GENY_DATA.RACE_HORSES as rdata
		left join GENY_DATA.RACES as summary on summary.RACE_ID=rdata.RACE_ID
		left join GENY_DATA.RACE_RESULTS as rslt on rdata.RACE_ID=rslt.RACE_ID and rdata.POS_DEPART=rslt.POS_DEPART
		left join GENY_DATA.RACE_COTES as cotes on rdata.RACE_ID=cotes.RACE_ID and rdata.POS_DEPART=cotes.POS_DEPART
		left join GENY_DATA.RACE_CASAQUES as casaques on rdata.RACE_ID=casaques.RACE_ID and rdata.POS_DEPART=casaques.H_POS,
        
		GENY_DATA.COTE_EVEOL as pic,
		GENY_DATA.OEILLERES as oei,
		GENY_DATA.IRON_REMOVAL as iron,
		GENY_DATA.CASAQUES as casaq
	where
		summary.RACE_ID is not null		and
		rdata.HORSE_ID is not null		and
		iron.ID = rdata.IRON_REMOV		and
		oei.ID  = rdata.OEIL 	  		and
		pic.ID = cotes.EVOL 			and
		casaq.LNK = casaques.LNK		and
        summary.RACE_ID is not null		and
        rdata.HORSE_ID is not null;
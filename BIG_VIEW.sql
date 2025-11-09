drop VIEW if exists GENY_DATA.BIG_VIEW;
CREATE VIEW BIG_VIEW AS
	select sql_cache
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
		-- GENY_DATA.RACE_RESULTS as rslt,
		-- GENY_DATA.RACE_COTES as cotes,
		GENY_DATA.COTE_EVEOL as pic,
		GENY_DATA.OEILLERES as oei,
		GENY_DATA.IRON_REMOVAL as iron,
		GENY_DATA.CASAQUES as casaq
	where
		iron.ID = rdata.IRON_REMOV		and
		oei.ID  = rdata.OEIL 	  		and
		pic.ID = cotes.EVOL 			and
		casaq.LNK = casaques.LNK;
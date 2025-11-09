delimiter //

drop procedure if exists GENY_DATA.RACE_SUMMARY //
create procedure GENY_DATA.RACE_SUMMARY(in rid int)
begin
	select 
		DT as DATE,
        HYPPO,
        R_NAME,
		RACE_ID,
		TIME_FORMAT(H_DEPART, "%h:%i") as DEPART,
   		CONCAT(COALESCE(PISTE, ""), " ", COALESCE(SPEC, "")) as PIST,
		CONCAT(TERRAIN, " ", COALESCE(ET_GAZON, "")) as TERRAIN,
		TYPE_C,
        SPECIALITE,
		COURSE,
        categorie,
		CORDE,
		PRIZE,
		R_LENGHT,
		ELIGIBLES,
		REPLACE(DETAILS_COURSE, "#", " - ") as DETAILS_COURSE,
		CONCAT("<a href='", STAT_LNK,"'>", STAT_LNK, "</a>") as RLINK
	from
		GENY_DATA.RACES
    where RACE_ID=rid;
end //

drop procedure if exists GENY_DATA.RACE_DATA //
create procedure GENY_DATA.RACE_DATA(in rid int)
begin
	select 
		rdata.HORSE_ID,
        casaq.IMG as CASAQ,
		rdata.POS_DEPART as P,
		rdata.HORSE,
		DRIVER,
		COACH,
		oei.DESCR as OEIL,
		HR_LENGTH,
		iron.DESCR as IRON,
		DECHARGE as DECH,
		CONCAT(SEX, AGE) as SEX_AGE,
		WEIGHT,
		DRIVER_ID,
		COACH_ID,
		MUSIC,
		WINNING,
		VALEUR,
		cotes.COTE_30mn,
		pic.IMG as EVOL,
		cotes.COTE_F,
		rslt.UNIT_TIME,
		rslt.VISUAL_GAP,
		rslt.FAIL,
		rslt.RANK
	from
		GENY_DATA.RACE_HORSES as rdata
        left join GENY_DATA.RACES as summary on summary.RACE_ID=rdata.RACE_ID
        left join GENY_DATA.RACE_RESULTS as rslt on rdata.RACE_ID=rslt.RACE_ID and rdata.POS_DEPART=rslt.POS_DEPART
        left join GENY_DATA.RACE_COTES as cotes on rdata.RACE_ID=cotes.RACE_ID and rdata.POS_DEPART=cotes.POS_DEPART
        left join GENY_DATA.RACE_CASAQUES as casaques on rdata.RACE_ID=casaques.RACE_ID and rdata.POS_DEPART=casaques.H_POS
        left join GENY_DATA.COTE_EVEOL as pic on pic.ID = cotes.EVOL
        left join GENY_DATA.OEILLERES as oei on oei.ID  = rdata.OEIL
        left join GENY_DATA.IRON_REMOVAL as iron on iron.ID = rdata.IRON_REMOV
        left join GENY_DATA.CASAQUES as casaq on casaq.LNK = casaques.LNK
        
	where
        rdata.RACE_ID=rid
	order by rdata.POS_DEPART;
end //

drop procedure if exists GENY_DATA.MULTI_HIST //
create procedure GENY_DATA.MULTI_HIST(
		in hid INT,
        in jid INT,
        in cid INT,
        in dtx DATE
    )
begin

	SELECT 
		DATE_FORMAT(summary.DT, "%Y-%m-%d") as DT,
		CONCAT(HYPPO, " ", CONCAT(COALESCE(PISTE, ""), " ", COALESCE(SPEC, ""))) as HYPPO,
		SPECIALITE,
		COURSE,
		categorie as CATEGORY,
		CORDE,
		PRIZE,
		CONCAT(TERRAIN, " ", COALESCE(ET_GAZON, "")) as TERRAIN,
		SPEC,
		R_LENGHT as LENGHT,
		HR_LENGTH as H_LEN,
		rdata.POS_DEPART as P,
        rdata.HORSE,
        HORSE_ID,
		DRIVER,
        DRIVER_ID,
		COACH,
        COACH_ID,
		iron.DESCR as IRON,
		oei.DESCR as OEIL, 
		DECHARGE as DECH,
		WEIGHT,
		VALEUR,
		RANK,
		FAIL,
		UNIT_TIME as PERF,
		VISUAL_GAP as GAP,
		COTE_30mn as COTE_I,
		pic.IMG as EVOL,
		COTE_F
	
    from		
		GENY_DATA.RACES as summary
		left join GENY_DATA.RACE_HORSES as rdata on rdata.RACE_ID=summary.RACE_ID and summary.DT < dtx and
        (
			rdata.HORSE_ID=hid or
			rdata.COACH_ID=cid or
			rdata.DRIVER_ID=jid
		)
		left join GENY_DATA.RACE_RESULTS as rslt on rdata.RACE_ID=rslt.RACE_ID and rdata.POS_DEPART=rslt.POS_DEPART
		left join GENY_DATA.RACE_COTES as cotes on rdata.RACE_ID=cotes.RACE_ID and rdata.POS_DEPART=cotes.POS_DEPART
		left join GENY_DATA.RACE_CASAQUES as casaques on rdata.RACE_ID=casaques.RACE_ID and rdata.POS_DEPART=casaques.H_POS,
		GENY_DATA.COTE_EVEOL as pic,
		GENY_DATA.OEILLERES as oei,
		GENY_DATA.IRON_REMOVAL as iron,
		GENY_DATA.CASAQUES as casaq
	where
		iron.ID = rdata.IRON_REMOV		and
		oei.ID  = rdata.OEIL 	  		and
		pic.ID = cotes.EVOL 			and
		casaq.LNK = casaques.LNK		
		
	order by DT desc;
end//


drop procedure if exists GENY_DATA.DUELS //
create procedure GENY_DATA.DUELS(
in horse1 INT,
in horse2 INT,
in dte DATE
)
begin

	SELECT 
		RACE_ID,
		DATE_FORMAT(DT, "%Y-%m-%d") as DT,
		CONCAT(HYPPO, " ", CONCAT(COALESCE(PISTE, ""), " ", COALESCE(SPEC, ""))) as HYPPO,
		SPECIALITE,
		COURSE,
		categorie as CATEGORY,
		CORDE,
		PRIZE,
		CONCAT(TERRAIN, " ", COALESCE(ET_GAZON, "")) as TERRAIN,
		SPEC,
		R_LENGHT as LENGHT,
		HR_LENGTH as H_LEN,
		POS_DEPART as P,
        HORSE,
        HORSE_ID,
		DRIVER,
        DRIVER_ID,
		COACH,
        COACH_ID,
		IRON_DESC as IRON,
		OEIL_DESC as OEIL, 
		DECHARGE as DECH,
		WEIGHT,
		VALEUR,
		RANK,
		FAIL,
		UNIT_TIME as PERF,
		VISUAL_GAP as GAP,
		COTE_30mn as COTE_I,
		EVOL_IMG as EVOL,
		COTE_F
	from 
		GENY_DATA.BIG_VIEW 
    where 
		RACE_ID in 
		(
			select distinct a.RACE_ID from GENY_DATA.RACE_HORSES as a
			inner join (select RACE_ID from GENY_DATA.RACE_HORSES where HORSE_ID=horse1) as b on a.RACE_ID = b.RACE_ID
			where HORSE_ID=horse2 and a.DT < dte
		) 
		and HORSE_ID in (horse1, horse2);
        
end //


drop procedure if exists GENY_DATA.ALL_DUELS //
create procedure GENY_DATA.ALL_DUELS(
in rid INT
)
begin

	SELECT 
		RACE_ID,
		DATE_FORMAT(DT, "%Y-%m-%d") as DT,
		CONCAT(HYPPO, " ", CONCAT(COALESCE(PISTE, ""), " ", COALESCE(SPEC, ""))) as HYPPO,
		SPECIALITE,
		COURSE,
		categorie as CATEGORY,
		CORDE,
		PRIZE,
		CONCAT(TERRAIN, " ", COALESCE(ET_GAZON, "")) as TERRAIN,
		SPEC,
		R_LENGHT as LENGHT,
		HR_LENGTH as H_LEN,
		POS_DEPART as P,
        HORSE,
        HORSE_ID,
		DRIVER,
        DRIVER_ID,
		COACH,
        COACH_ID,
		IRON_DESC as IRON,
		OEIL_DESC as OEIL, 
		DECHARGE as DECH,
		WEIGHT,
		VALEUR,
		RANK,
		FAIL,
		UNIT_TIME as PERF,
		VISUAL_GAP as GAP,
		COTE_30mn as COTE_I,
		EVOL_IMG as EVOL,
		COTE_F
	from 
		GENY_DATA.BIG_VIEW 
    where 
		RACE_ID in 
		(
			select b.RACE_ID from
			(
				select sum(elem) as NB_H, RACE_ID from 
				(
					select 1 as elem, g.RACE_ID from GENY_DATA.RACE_HORSES as g
					where 
					g.HORSE_ID in (select distinct r.HORSE_ID from GENY_DATA.RACE_HORSES as r where r.RACE_ID=rid) 
					and g.DT < (select distinct d.DT from GENY_DATA.RACES as d where d.RACE_ID=rid)
				) as a group by a.RACE_ID
			) as b where b.NB_H > 1
		) 
		and HORSE_ID in (select distinct r.HORSE_ID from GENY_DATA.RACE_HORSES as r where r.RACE_ID=rid);
        
end //


-- 
-- drop procedure if exists GENY_DATA.race_coaches //
-- create procedure GENY_DATA.race_coaches(
-- in dt1 VARCHAR(100),
-- in hyppo1 VARCHAR(100),
-- in r_name1 VARCHAR(100)
-- )
-- begin
-- 
-- 	select distinct COACH
-- 	from GENY_DATA.OBSERVATIONS
--     where 
--         DT=dt1
--         and HYPPO=hyppo1
--         and R_NAME=r_name1;
--         
-- end //
-- 
-- drop procedure if exists GENY_DATA.perf_details //
-- create procedure GENY_DATA.perf_details(
-- in dt1 VARCHAR(100),
-- in hyppo1 VARCHAR(100),
-- in r_name1 VARCHAR(100),
-- in horse1 VARCHAR(100)
-- )
-- begin
-- 
-- 	select *
-- 	from GENY_DATA.OBSERVATIONS
--     where 
--         DT=dt1
--         and HYPPO=hyppo1
--         and R_NAME=r_name1
--         and HORSE=horse1;
--         
-- end //
-- 
-- drop procedure if exists GENY_DATA.horse_hist //
-- create procedure GENY_DATA.horse_hist(
-- in horse1 VARCHAR(100)
-- )
-- begin
-- 
-- 	select *
-- 	from GENY_DATA.OBSERVATIONS
--     where 
--         HORSE=horse1;
--         
-- end //
-- 
-- drop procedure if exists GENY_DATA.driver_hist //
-- create procedure GENY_DATA.driver_hist(
-- in driver1 VARCHAR(100)
-- )
-- begin
-- 
-- 	select *
-- 	from GENY_DATA.OBSERVATIONS
--     where 
--         DRIVER=driver1;
--         
-- end //
-- 
-- drop procedure if exists GENY_DATA.coach_hist //
-- create procedure GENY_DATA.coach_hist(
-- in coach1 VARCHAR(100)
-- )
-- begin
-- 
-- 	select *
-- 	from GENY_DATA.OBSERVATIONS
--     where 
--         COACH=coach1;
--         
-- end //
-- 
-- drop procedure if exists GENY_DATA.triplet_hist //
-- create procedure GENY_DATA.triplet_hist(
-- in horse1 VARCHAR(100),
-- in driver1 VARCHAR(100),
-- in coach1 VARCHAR(100)
-- )
-- begin
-- 
-- 	select *
-- 	from GENY_DATA.OBSERVATIONS
--     where 
--         COACH=coach1
--         and DRIVER=driver1
--         and HORSE=horse1;
--         
-- end //
-- 

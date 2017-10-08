--
SET statement_timeout = 0;
SET lock_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET row_security = off;

--
-- Name: plpgsql; Type: EXTENSION; Schema: -; Owner: 
--

CREATE EXTENSION IF NOT EXISTS plpgsql WITH SCHEMA pg_catalog;


--
-- Name: EXTENSION plpgsql; Type: COMMENT; Schema: -; Owner: 
--

COMMENT ON EXTENSION plpgsql IS 'PL/pgSQL procedural language';


SET search_path = public, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: alliance; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE alliance (
    sname character varying(30) NOT NULL,
    kleader character varying(30),
    bmobrun boolean,
    ncredits integer,
    nhomesector integer,
    kterritory integer
);


ALTER TABLE alliance OWNER TO postgres;

--
-- Name: config; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE config (
    sname character varying(60) NOT NULL,
    svalue character varying(128)
);


ALTER TABLE config OWNER TO postgres;

COPY config (sname, svalue) FROM stdin WITH DELIMITER ',';
gap_ratio,0.1
turns_per_day,1000
base_carbon_price,20
base_metals_price,30
base_plasma_price,50
stock_unit,2000
price_change_delay,1440
buy_price_delta_per_unit,0.85
point_loss_ship_destroy,500
point_gain_ship_destroy,1000
point_gain_kill,1500
point_loss_kill,2000
escape_pod_nkey,10
log_level,3
sentinels_per_sector,15000
hold_price,1000
sentinel_price,750
missile_price,300
shield_price,200
mean_missile_dmg,2
missile_dmg_deviation,1.75
sentinel_dmg_deviation,1.75
mean_sentinel_dmg,4
price_floor_multiplier,0.8
price_cap_multiplier,1.4
stock_recovery_minutes,10080
sell_price_delta_per_unit,1.25
sellrate_linear_delta,4
buyrate_cap,75
sellrate_floor,20
idle_timeout_secs,600
\.


--
-- Name: edges; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE edges (
    nsector integer NOT NULL,
    nedge integer NOT NULL,
    nsector2 integer NOT NULL
);


ALTER TABLE edges OWNER TO postgres;

--
-- Name: eventlog; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE eventlog (
    dstamp timestamp without time zone NOT NULL,
    sactor character varying(30),
    ssubject character varying(30),
    ntype integer NOT NULL,
    smessage character varying(80),
    kshiptye integer,
    sshipname character varying(30)
);


ALTER TABLE eventlog OWNER TO postgres;

--
-- Name: log; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE log (
    dstamp timestamp without time zone NOT NULL,
    nseverity integer NOT NULL,
    smessage text NOT NULL,
    sfilename text,
    nline integer,
    CONSTRAINT log_nseverity_check CHECK (((nseverity >= 0) AND (nseverity < 6)))
);


ALTER TABLE log OWNER TO postgres;

--
-- Name: login; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE login (
    slogin character varying(20) NOT NULL,
    spassword character varying(256) NOT NULL,
    dfirstlogin timestamp without time zone,
    dlastlogin timestamp without time zone,
    slastip character varying(20),
    semail character varying(80),
    bdisabled boolean,
    nlogins integer
);


ALTER TABLE login OWNER TO postgres;

--
-- Name: mail; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE mail (
    nmailid integer NOT NULL,
    dstamp timestamp without time zone NOT NULL,
    ktoplayer character varying(30) NOT NULL,
    kfromplayer character varying(30),
    bfromsystem boolean,
    smessage character varying(1024)
);


ALTER TABLE mail OWNER TO postgres;

--
-- Name: mail_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE mail_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
    CYCLE;


ALTER TABLE mail_id_seq OWNER TO postgres;

--
-- Name: mob; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE mob (
    sname character varying(30) NOT NULL,
    npersonality integer,
    nhomesector integer,
    ndest integer,
    CONSTRAINT mob_npersonality_check CHECK (((npersonality >= 0) AND (npersonality <= 6)))
);


ALTER TABLE mob OWNER TO postgres;

--
-- Name: news; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE news (
    nkey integer NOT NULL,
    dposted timestamp without time zone NOT NULL,
    binactive boolean,
    burgent boolean,
    smessage text NOT NULL
);


ALTER TABLE news OWNER TO postgres;

--
-- Name: outpost; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE outpost (
    sname character varying(60) NOT NULL,
    ksector integer NOT NULL,
    bspecial boolean,
    bbuyplasma boolean NOT NULL,
    bbuymetals boolean NOT NULL,
    bbuycarbon boolean NOT NULL,
    kdiscoverer character varying(30),
    klastvisitor character varying(30),
    dlastvisit timestamp without time zone,
    ndaystocompletion integer,
    fplasmaprice real,
    fmetalsprice real,
    fcarbonprice real
);


ALTER TABLE outpost OWNER TO postgres;

--
-- Name: player; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE player (
    sname character varying(30) NOT NULL,
    klogin character varying(20) NOT NULL,
    bmob boolean,
    ncredits integer,
    nturnsleft integer,
    npoints integer,
    kalliance character varying(30),
    kcurrentship integer,
    dfirstplay timestamp without time zone,
    dlastplay timestamp without time zone,
    bdead boolean
);


ALTER TABLE player OWNER TO postgres;

--
-- Name: sectors; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE sectors (
    nsector integer NOT NULL,
    kterritory integer,
    bdiscovered boolean,
    bstardock boolean,
    sstardockname character varying(60),
    fx real,
    fy real
);


ALTER TABLE sectors OWNER TO postgres;

--
-- Name: sentinels; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE sentinels (
    ksector integer NOT NULL,
    kplayer character varying(30) NOT NULL,
    ncount integer NOT NULL
);


ALTER TABLE sentinels OWNER TO postgres;

--
-- Name: ship; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE ship (
    nkey integer NOT NULL,
    sname character varying(30) NOT NULL,
    ktype integer NOT NULL,
    ballianceowned boolean,
    kowner character varying(30),
    kalliance character varying(30),
    bpersonal boolean,
    nsentinels integer,
    nmissiles integer,
    nmines integer,
    ntrackers integer,
    nshields integer,
    nplasma integer,
    nmetals integer,
    ncarbon integer,
    npeople integer,
    ndebris integer,
    nexplosives integer,
    nprobes integer,
    nholds integer,
    bcloaked boolean,
    ktowship integer,
    ksector integer
);


ALTER TABLE ship OWNER TO postgres;

--
-- Name: ship_nkey_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE ship_nkey_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    MAXVALUE 99999
    CACHE 1
    CYCLE;


ALTER TABLE ship_nkey_seq OWNER TO postgres;

--
-- Name: shipmanufacturer; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE shipmanufacturer (
    nkey integer NOT NULL,
    sname character varying(35) NOT NULL
);


ALTER TABLE shipmanufacturer OWNER TO postgres;

--
-- Name: shiptype; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE shiptype (
    nkey integer NOT NULL,
    kmanufacturer integer NOT NULL,
    sname character varying(30) NOT NULL,
    nmaxmissiles integer,
    nmaxshields integer,
    nmaxattack integer,
    nmaxholds integer,
    ninitholds integer,
    nmaxsentinels integer,
    nmaxtrackers integer,
    nmaxmines integer,
    nmaxpeople integer,
    nmaxprobes integer,
    nturnspersector integer,
    bwarpdrive boolean,
    bcloakable boolean,
    banalyzer boolean,
    nscandistance integer,
    ncost integer,
    nforecolor integer,
    nbackcolor integer,
    ntransrange integer,
    bforsale boolean,
    CONSTRAINT shiptype_check CHECK ((ninitholds <= nmaxholds)),
    CONSTRAINT shiptype_nbackcolor_check CHECK (((nbackcolor >= 0) AND (nbackcolor <= 7))),
    CONSTRAINT shiptype_nforecolor_check CHECK (((nforecolor >= 0) AND (nforecolor <= 15)))
);


ALTER TABLE shiptype OWNER TO postgres;

--
-- Name: territory; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE territory (
    nkey integer NOT NULL,
    sname character varying(128) NOT NULL
);


ALTER TABLE territory OWNER TO postgres;




ALTER TABLE ONLY alliance
    ADD CONSTRAINT alliance_pkey PRIMARY KEY (sname);



--
-- Name: config_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY config
    ADD CONSTRAINT config_pkey PRIMARY KEY (sname);


--
-- Name: edges_nsector_nedge_key; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY edges
    ADD CONSTRAINT edges_nsector_nedge_key UNIQUE (nsector, nedge);


--
-- Name: login_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY login
    ADD CONSTRAINT login_pkey PRIMARY KEY (slogin);


--
-- Name: mail_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY mail
    ADD CONSTRAINT mail_pkey PRIMARY KEY (nmailid);


--
-- Name: mob_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY mob
    ADD CONSTRAINT mob_pkey PRIMARY KEY (sname);


--
-- Name: news_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY news
    ADD CONSTRAINT news_pkey PRIMARY KEY (nkey);


--
-- Name: outpost_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY outpost
    ADD CONSTRAINT outpost_pkey PRIMARY KEY (sname);


--
-- Name: player_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY player
    ADD CONSTRAINT player_pkey PRIMARY KEY (sname);


--
-- Name: sectors_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY sectors
    ADD CONSTRAINT sectors_pkey PRIMARY KEY (nsector);


--
-- Name: sentinels_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY sentinels
    ADD CONSTRAINT sentinels_pkey PRIMARY KEY (ksector, kplayer);


--
-- Name: ship_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY ship
    ADD CONSTRAINT ship_pkey PRIMARY KEY (nkey);


--
-- Name: shipmanufacturer_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY shipmanufacturer
    ADD CONSTRAINT shipmanufacturer_pkey PRIMARY KEY (nkey);


--
-- Name: shiptype_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY shiptype
    ADD CONSTRAINT shiptype_pkey PRIMARY KEY (nkey);


--
-- Name: territory_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY territory
    ADD CONSTRAINT territory_pkey PRIMARY KEY (nkey);


--
-- Name: territory_sname_key; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY territory
    ADD CONSTRAINT territory_sname_key UNIQUE (sname);



--
-- Name: eventlog_dstamp; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX eventlog_dstamp ON eventlog USING btree (dstamp);


--
-- Name: mail_dstamp_idx; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX mail_dstamp_idx ON mail USING btree (dstamp);


--
-- Name: mail_id_idx; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX mail_id_idx ON mail USING btree (nmailid);


--
-- Name: mail_to_idx; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX mail_to_idx ON mail USING btree (ktoplayer);


--
-- Name: nsector2_idx; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX nsector2_idx ON edges USING btree (nsector2);


--
-- Name: nsector_idx; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX nsector_idx ON edges USING btree (nsector);


--
-- Name: sent_plyr_idx; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX sent_plyr_idx ON sentinels USING btree (kplayer);


--
-- Name: sent_sec_idx; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX sent_sec_idx ON sentinels USING btree (ksector);


--
-- Name: ship_ktow_idx; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX ship_ktow_idx ON ship USING btree (ktowship);


--
-- Name: alliance_kleader_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY alliance
    ADD CONSTRAINT alliance_kleader_fkey FOREIGN KEY (kleader) REFERENCES player(sname);


--
-- Name: alliance_kterritory_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY alliance
    ADD CONSTRAINT alliance_kterritory_fkey FOREIGN KEY (kterritory) REFERENCES territory(nkey);


--
-- Name: alliance_nhomesector_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY alliance
    ADD CONSTRAINT alliance_nhomesector_fkey FOREIGN KEY (nhomesector) REFERENCES sectors(nsector);


--
-- Name: edges_nsector2_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY edges
    ADD CONSTRAINT edges_nsector2_fkey FOREIGN KEY (nsector2) REFERENCES sectors(nsector);


--
-- Name: edges_nsector_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY edges
    ADD CONSTRAINT edges_nsector_fkey FOREIGN KEY (nsector) REFERENCES sectors(nsector);


--
-- Name: eventlog_kshiptye_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY eventlog
    ADD CONSTRAINT eventlog_kshiptye_fkey FOREIGN KEY (kshiptye) REFERENCES shiptype(nkey);


--
-- Name: eventlog_sactor_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY eventlog
    ADD CONSTRAINT eventlog_sactor_fkey FOREIGN KEY (sactor) REFERENCES player(sname);


--
-- Name: mail_kfromplayer_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY mail
    ADD CONSTRAINT mail_kfromplayer_fkey FOREIGN KEY (kfromplayer) REFERENCES player(sname);


--
-- Name: mail_ktoplayer_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY mail
    ADD CONSTRAINT mail_ktoplayer_fkey FOREIGN KEY (ktoplayer) REFERENCES player(sname);


--
-- Name: mob_ndest_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY mob
    ADD CONSTRAINT mob_ndest_fkey FOREIGN KEY (ndest) REFERENCES sectors(nsector);


--
-- Name: mob_nhomesector_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY mob
    ADD CONSTRAINT mob_nhomesector_fkey FOREIGN KEY (nhomesector) REFERENCES sectors(nsector);


--
-- Name: outpost_kdiscoverer_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY outpost
    ADD CONSTRAINT outpost_kdiscoverer_fkey FOREIGN KEY (kdiscoverer) REFERENCES player(sname);


--
-- Name: outpost_klastvisitor_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY outpost
    ADD CONSTRAINT outpost_klastvisitor_fkey FOREIGN KEY (klastvisitor) REFERENCES player(sname);


--
-- Name: outpost_ksector_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY outpost
    ADD CONSTRAINT outpost_ksector_fkey FOREIGN KEY (ksector) REFERENCES sectors(nsector);


--
-- Name: player_kalliance_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY player
    ADD CONSTRAINT player_kalliance_fkey FOREIGN KEY (kalliance) REFERENCES alliance(sname);


--
-- Name: player_kcurrentship_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY player
    ADD CONSTRAINT player_kcurrentship_fkey FOREIGN KEY (kcurrentship) REFERENCES ship(nkey);


--
-- Name: player_klogin_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY player
    ADD CONSTRAINT player_klogin_fkey FOREIGN KEY (klogin) REFERENCES login(slogin);


--
-- Name: sectors_kterritory_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY sectors
    ADD CONSTRAINT sectors_kterritory_fkey FOREIGN KEY (kterritory) REFERENCES territory(nkey);


--
-- Name: sentinels_kplayer_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY sentinels
    ADD CONSTRAINT sentinels_kplayer_fkey FOREIGN KEY (kplayer) REFERENCES player(sname);


--
-- Name: sentinels_ksector_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY sentinels
    ADD CONSTRAINT sentinels_ksector_fkey FOREIGN KEY (ksector) REFERENCES sectors(nsector);


--
-- Name: ship_kalliance_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY ship
    ADD CONSTRAINT ship_kalliance_fkey FOREIGN KEY (kalliance) REFERENCES alliance(sname);


--
-- Name: ship_kowner_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY ship
    ADD CONSTRAINT ship_kowner_fkey FOREIGN KEY (kowner) REFERENCES player(sname);


--
-- Name: ship_ksector_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY ship
    ADD CONSTRAINT ship_ksector_fkey FOREIGN KEY (ksector) REFERENCES sectors(nsector);


--
-- Name: ship_ktowship_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY ship
    ADD CONSTRAINT ship_ktowship_fkey FOREIGN KEY (ktowship) REFERENCES ship(nkey);


--
-- Name: ship_ktype_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY ship
    ADD CONSTRAINT ship_ktype_fkey FOREIGN KEY (ktype) REFERENCES shiptype(nkey);


--
-- Name: shiptype_kmanufacturer_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY shiptype
    ADD CONSTRAINT shiptype_kmanufacturer_fkey FOREIGN KEY (kmanufacturer) REFERENCES shipmanufacturer(nkey);


--
-- Name: public; Type: ACL; Schema: -; Owner: postgres
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
REVOKE ALL ON SCHEMA public FROM postgres;
GRANT ALL ON SCHEMA public TO postgres;
GRANT ALL ON SCHEMA public TO PUBLIC;


--
-- Name: alliance; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE alliance FROM PUBLIC;
REVOKE ALL ON TABLE alliance FROM postgres;
GRANT ALL ON TABLE alliance TO postgres;
GRANT ALL ON TABLE alliance TO void;


--
-- Name: config; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE config FROM PUBLIC;
REVOKE ALL ON TABLE config FROM postgres;
GRANT ALL ON TABLE config TO postgres;
GRANT ALL ON TABLE config TO void;


--
-- Name: edges; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE edges FROM PUBLIC;
REVOKE ALL ON TABLE edges FROM postgres;
GRANT ALL ON TABLE edges TO postgres;
GRANT ALL ON TABLE edges TO void;


--
-- Name: eventlog; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE eventlog FROM PUBLIC;
REVOKE ALL ON TABLE eventlog FROM postgres;
GRANT ALL ON TABLE eventlog TO postgres;
GRANT ALL ON TABLE eventlog TO void;


--
-- Name: log; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE log FROM PUBLIC;
REVOKE ALL ON TABLE log FROM postgres;
GRANT ALL ON TABLE log TO postgres;
GRANT ALL ON TABLE log TO void;


--
-- Name: login; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE login FROM PUBLIC;
REVOKE ALL ON TABLE login FROM postgres;
GRANT ALL ON TABLE login TO postgres;
GRANT ALL ON TABLE login TO void;


--
-- Name: mail; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE mail FROM PUBLIC;
REVOKE ALL ON TABLE mail FROM postgres;
GRANT ALL ON TABLE mail TO postgres;
GRANT ALL ON TABLE mail TO void;


--
-- Name: mail_id_seq; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON SEQUENCE mail_id_seq FROM PUBLIC;
REVOKE ALL ON SEQUENCE mail_id_seq FROM postgres;
GRANT ALL ON SEQUENCE mail_id_seq TO postgres;
GRANT ALL ON SEQUENCE mail_id_seq TO void;


--
-- Name: mob; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE mob FROM PUBLIC;
REVOKE ALL ON TABLE mob FROM postgres;
GRANT ALL ON TABLE mob TO postgres;
GRANT ALL ON TABLE mob TO void;


--
-- Name: news; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE news FROM PUBLIC;
REVOKE ALL ON TABLE news FROM postgres;
GRANT ALL ON TABLE news TO postgres;
GRANT ALL ON TABLE news TO void;


--
-- Name: outpost; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE outpost FROM PUBLIC;
REVOKE ALL ON TABLE outpost FROM postgres;
GRANT ALL ON TABLE outpost TO postgres;
GRANT ALL ON TABLE outpost TO void;


--
-- Name: player; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE player FROM PUBLIC;
REVOKE ALL ON TABLE player FROM postgres;
GRANT ALL ON TABLE player TO postgres;
GRANT ALL ON TABLE player TO void;


--
-- Name: sectors; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE sectors FROM PUBLIC;
REVOKE ALL ON TABLE sectors FROM postgres;
GRANT ALL ON TABLE sectors TO postgres;
GRANT ALL ON TABLE sectors TO void;


--
-- Name: sentinels; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE sentinels FROM PUBLIC;
REVOKE ALL ON TABLE sentinels FROM postgres;
GRANT ALL ON TABLE sentinels TO postgres;
GRANT ALL ON TABLE sentinels TO void;


--
-- Name: ship; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE ship FROM PUBLIC;
REVOKE ALL ON TABLE ship FROM postgres;
GRANT ALL ON TABLE ship TO postgres;
GRANT ALL ON TABLE ship TO void;


--
-- Name: ship_nkey_seq; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON SEQUENCE ship_nkey_seq FROM PUBLIC;
REVOKE ALL ON SEQUENCE ship_nkey_seq FROM postgres;
GRANT ALL ON SEQUENCE ship_nkey_seq TO postgres;
GRANT ALL ON SEQUENCE ship_nkey_seq TO void;


--
-- Name: shipmanufacturer; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE shipmanufacturer FROM PUBLIC;
REVOKE ALL ON TABLE shipmanufacturer FROM postgres;
GRANT ALL ON TABLE shipmanufacturer TO postgres;
GRANT ALL ON TABLE shipmanufacturer TO void;


--
-- Name: shiptype; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE shiptype FROM PUBLIC;
REVOKE ALL ON TABLE shiptype FROM postgres;
GRANT ALL ON TABLE shiptype TO postgres;
GRANT ALL ON TABLE shiptype TO void;


--
-- Name: territory; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE territory FROM PUBLIC;
REVOKE ALL ON TABLE territory FROM postgres;
GRANT ALL ON TABLE territory TO postgres;
GRANT ALL ON TABLE territory TO void;



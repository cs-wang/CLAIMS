create table t1 (
        a1 char(64), 
        a2 char(64), 
        b char(16), 
        c char(16) ,
        d char(16) 
);

create projection on t1(a1, a2, b, c, d) number = 1 partitioned on a1; 

insert into t1 (a1, a2, b, c, d) values
('a','a','a','a111','xy1'),('a','a','a','b111','xy2'),('a','a','a','c111','xy3'),('a','a','a','d111','xy4'),
('a','a','b','e112','xy1'),('a','a','b','f112','xy2'),('a','a','b','g112','xy3'),('a','a','b','h112','xy4'),
('a','b','a','i121','xy1'),('a','b','a','j121','xy2'),('a','b','a','k121','xy3'),('a','b','a','l121','xy4'),
('a','b','b','m122','xy1'),('a','b','b','n122','xy2'),('a','b','b','o122','xy3'),('a','b','b','p122','xy4');
insert into t1 (a1, a2, b, c, d) values
('b','a','a','a211','xy1'),('b','a','a','b211','xy2'),('b','a','a','c211','xy3'),('b','a','a','d211','xy4'),
('b','a','b','e212','xy1'),('b','a','b','f212','xy2'),('b','a','b','g212','xy3'),('b','a','b','h212','xy4'),
('b','b','a','i221','xy1'),('b','b','a','j221','xy2'),('b','b','a','k221','xy3'),('b','b','a','l221','xy4'),
('b','b','b','m222','xy1'),('b','b','b','n222','xy2'),('b','b','b','o222','xy3'),('b','b','b','p222','xy4');
insert into t1 (a1, a2, b, c, d) values
('c','a','a','a311','xy1'),('c','a','a','b311','xy2'),('c','a','a','c311','xy3'),('c','a','a','d311','xy4'),
('c','a','b','e312','xy1'),('c','a','b','f312','xy2'),('c','a','b','g312','xy3'),('c','a','b','h312','xy4'),
('c','b','a','i321','xy1'),('c','b','a','j321','xy2'),('c','b','a','k321','xy3'),('c','b','a','l321','xy4'),
('c','b','b','m322','xy1'),('c','b','b','n322','xy2'),('c','b','b','o322','xy3'),('c','b','b','p322','xy4');
insert into t1 (a1, a2, b, c, d) values
('d','a','a','a411','xy1'),('d','a','a','b411','xy2'),('d','a','a','c411','xy3'),('d','a','a','d411','xy4'),
('d','a','b','e412','xy1'),('d','a','b','f412','xy2'),('d','a','b','g412','xy3'),('d','a','b','h412','xy4'),
('d','b','a','i421','xy1'),('d','b','a','j421','xy2'),('d','b','a','k421','xy3'),('d','b','a','l421','xy4'),
('d','b','b','m422','xy1'),('d','b','b','n422','xy2'),('d','b','b','o422','xy3'),('d','b','b','p422','xy4');
insert into t1 (a1, a2, b, c, d) values
('a','a','a','a111','xy1'),('a','a','a','b111','xy2'),('a','a','a','c111','xy3'),('a','a','a','d111','xy4'),
('a','a','b','e112','xy1'),('a','a','b','f112','xy2'),('a','a','b','g112','xy3'),('a','a','b','h112','xy4'),
('a','b','a','i121','xy1'),('a','b','a','j121','xy2'),('a','b','a','k121','xy3'),('a','b','a','l121','xy4'),
('a','b','b','m122','xy1'),('a','b','b','n122','xy2'),('a','b','b','o122','xy3'),('a','b','b','p122','xy4');
insert into t1 (a1, a2, b, c, d) values
('b','a','a','a211','xy1'),('b','a','a','b211','xy2'),('b','a','a','c211','xy3'),('b','a','a','d211','xy4'),
('b','a','b','e212','xy1'),('b','a','b','f212','xy2'),('b','a','b','g212','xy3'),('b','a','b','h212','xy4'),
('b','b','a','i221','xy1'),('b','b','a','j221','xy2'),('b','b','a','k221','xy3'),('b','b','a','l221','xy4'),
('b','b','b','m222','xy1'),('b','b','b','n222','xy2'),('b','b','b','o222','xy3'),('b','b','b','p222','xy4');
insert into t1 (a1, a2, b, c, d) values
('c','a','a','a311','xy1'),('c','a','a','b311','xy2'),('c','a','a','c311','xy3'),('c','a','a','d311','xy4'),
('c','a','b','e312','xy1'),('c','a','b','f312','xy2'),('c','a','b','g312','xy3'),('c','a','b','h312','xy4'),
('c','b','a','i321','xy1'),('c','b','a','j321','xy2'),('c','b','a','k321','xy3'),('c','b','a','l321','xy4'),
('c','b','b','m322','xy1'),('c','b','b','n322','xy2'),('c','b','b','o322','xy3'),('c','b','b','p322','xy4');
insert into t1 (a1, a2, b, c, d) values
('d','a','a','a411','xy1'),('d','a','a','b411','xy2'),('d','a','a','c411','xy3'),('d','a','a','d411','xy4'),
('d','a','b','e412','xy1'),('d','a','b','f412','xy2'),('d','a','b','g412','xy3'),('d','a','b','h412','xy4'),
('d','b','a','i421','xy1'),('d','b','a','j421','xy2'),('d','b','a','k421','xy3'),('d','b','a','l421','xy4'),
('d','b','b','m422','xy1'),('d','b','b','n422','xy2'),('d','b','b','o422','xy3'),('d','b','b','p422','xy4');

1. DISTINCT not support
2. max min string value not effective.
3. select a1,a2,b,max(c),min(c) from t1 where (a2 = 'a') and (b = 'b') group by a1; query case claimsserver Segmentation fault (core dumped).
4. select a1,a2,b,max(c),min(c) from t1 where (a2 = 'a') group by a1; query case claimsserver Segmentation fault (core dumped).
5. select a1,a2,b,       max(c) from t1 where (b = 'b') group by a1,a2;
6. select a1,a2,b,min(c) from t1 where (ord(a1) > 97) and (ord(a2) + ord(a1) > 194) and (b = 'c') group by a1,a2,b; failed
7. select a1,a2,b,c from t1 where (a2 >= 'b') and (b = 'a') and (c = 'i121') group by a1,a2,b;


-- Basic monitoring query
select c.id as configurationid, c.studyid, filename, replicateid, 
  starttime, now() - starttime as runningtime, 
  max(dayselapsed) as modeldays
from sim.replicate r
  inner join sim.configuration c on c.id = r.configurationid
  inner join sim.monthlydata md on md.replicateid = r.id
where r.endtime is null
group by c.id, filename, replicateid, starttime
order by modeldays desc

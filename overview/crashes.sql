-- number of failed runs by release

SELECT application_name, application_version, application_release, build_distribution, count(*) as count
FROM run NATURAL JOIN build
WHERE exit_signal != 0 
GROUP BY application_name, application_version, application_release, build_distribution

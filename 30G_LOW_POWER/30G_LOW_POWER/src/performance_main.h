
/* Prevent double inclusion */
#ifndef PERF_MAIN_H
#define PERF_MAIN_H

/*
 *	Multiple factor for timer peer mode
 */
#define PEER_MODE_INTERVAL_MULT_FACTOR       (10)
/*
 *	Timer values for the timer user in peer active mode
 */
#define PEER_MODE_INTERVAL_IN_MICRO_SEC      (1000000 * PEER_MODE_INTERVAL_MULT_FACTOR)


/**
 * \brief Init function of the Performance Analyzer application
 * \ingroup group_app_init
 */
void performance_analyzer_init(void);

/**
 * \brief This task needs to be called in a while(1) for performing Performance
 * Analyzer tasks
 */
void performance_analyzer_task(void);

#endif

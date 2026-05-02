#ifndef MDEBUG_H
#define MDEBUG_H


#ifdef __cplusplus
extern "C" {
#endif

#define funcDebug() qDebug() << __FUNCTION__

#ifdef __cplusplus
}
#endif
#endif // MDEBUG_H

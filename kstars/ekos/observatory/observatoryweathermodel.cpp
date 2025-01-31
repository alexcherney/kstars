/*  Ekos Observatory Module
    Copyright (C) Wolfgang Reissenberger <sterne-jaeger@t-online.de>

    This application is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
 */

#include "observatoryweathermodel.h"
#include "Options.h"
#include <KLocalizedString>

namespace Ekos
{

void ObservatoryWeatherModel::initModel(Weather *weather)
{
    weatherInterface = weather;

    // ensure that we start the timers if required
    weatherChanged(status());

    connect(weatherInterface, &Weather::ready, this, &ObservatoryWeatherModel::updateWeatherStatus);
    connect(weatherInterface, &Weather::newStatus, this, &ObservatoryWeatherModel::weatherChanged);
    connect(weatherInterface, &Weather::newWeatherData, this, &ObservatoryWeatherModel::updateWeatherData);
    connect(weatherInterface, &Weather::disconnected, this, &ObservatoryWeatherModel::disconnected);

    // read the default values
    warningActionsActive = Options::warningActionsActive();
    warningActions.parkDome = Options::weatherWarningCloseDome();
    warningActions.closeShutter = Options::weatherWarningCloseShutter();
    warningActions.delay = Options::weatherWarningDelay();
    warningActions.stopScheduler = Options::weatherAlertStopScheduler();
    alertActionsActive = Options::alertActionsActive();
    alertActions.parkDome = Options::weatherAlertCloseDome();
    alertActions.closeShutter = Options::weatherAlertCloseShutter();
    alertActions.stopScheduler = Options::weatherAlertStopScheduler();
    alertActions.delay = Options::weatherAlertDelay();

    warningTimer.setInterval(static_cast<int>(warningActions.delay * 1000));
    warningTimer.setSingleShot(true);
    alertTimer.setInterval(static_cast<int>(alertActions.delay * 1000));
    alertTimer.setSingleShot(true);

    connect(&warningTimer, &QTimer::timeout, [this]()
    {
        execute(warningActions);
    });
    connect(&alertTimer, &QTimer::timeout, [this]()
    {
        execute(alertActions);
    });

    if (weatherInterface->status() != ISD::Weather::WEATHER_IDLE)
        emit ready();
}

ISD::Weather::Status ObservatoryWeatherModel::status()
{
    if (weatherInterface == nullptr)
        return ISD::Weather::WEATHER_IDLE;

    return weatherInterface->status();
}

void ObservatoryWeatherModel::setWarningActionsActive(bool active)
{
    warningActionsActive = active;
    Options::setWarningActionsActive(active);

    // stop warning actions if deactivated
    if (!active && warningTimer.isActive())
        warningTimer.stop();
    // start warning timer if activated
    else if (active && !warningTimer.isActive() && weatherInterface->status() == ISD::Weather::WEATHER_WARNING)
        warningTimer.start();
}

void ObservatoryWeatherModel::setAlertActionsActive(bool active)
{
    alertActionsActive = active;
    Options::setAlertActionsActive(active);

    // stop alert actions if deactivated
    if (!active && alertTimer.isActive())
        alertTimer.stop();
    // start alert timer if activated
    else if (active && !alertTimer.isActive() && weatherInterface->status() == ISD::Weather::WEATHER_ALERT)
        alertTimer.start();
}

void ObservatoryWeatherModel::setWarningActions(WeatherActions actions)
{
    warningActions = actions;
    Options::setWeatherWarningCloseDome(actions.parkDome);
    Options::setWeatherWarningCloseShutter(actions.closeShutter);
    Options::setWeatherWarningDelay(actions.delay);
    warningTimer.setInterval(static_cast<int>(actions.delay * 1000));
}


QString ObservatoryWeatherModel::getWarningActionsStatus()
{
    if (warningTimer.isActive())
    {
        int remaining = warningTimer.remainingTime() / 1000;
        return i18np("%1 second remaining", "%1 seconds remaining", QString::number(remaining));
    }

    return i18n("Status: inactive");
}

void ObservatoryWeatherModel::setAlertActions(WeatherActions actions)
{
    alertActions = actions;
    Options::setWeatherAlertCloseDome(actions.parkDome);
    Options::setWeatherAlertCloseShutter(actions.closeShutter);
    Options::setWeatherAlertDelay(actions.delay);
    alertTimer.setInterval(static_cast<int>(actions.delay * 1000));
}

QString ObservatoryWeatherModel::getAlertActionsStatus()
{
    if (alertTimer.isActive())
    {
        int remaining = alertTimer.remainingTime() / 1000;
        return i18np("%1 second remaining", "%1 seconds remaining", QString::number(remaining));
    }

    return i18n("Status: inactive");
}

void ObservatoryWeatherModel::updateWeatherStatus()
{
    weatherChanged(status());
    emit ready();
}


void ObservatoryWeatherModel::weatherChanged(ISD::Weather::Status status)
{
    switch (status)
    {
        case ISD::Weather::WEATHER_OK:
            warningTimer.stop();
            alertTimer.stop();
            break;
        case ISD::Weather::WEATHER_WARNING:
            if (warningActionsActive)
                warningTimer.start();
            alertTimer.stop();
            break;
        case ISD::Weather::WEATHER_ALERT:
            warningTimer.stop();
            if (alertActionsActive)
                alertTimer.start();
            break;
        default:
            break;
    }
    emit newStatus(status);
}

void ObservatoryWeatherModel::updateWeatherData(std::vector<ISD::Weather::WeatherData> entries)
{
    // add or update all received values
    for (std::vector<ISD::Weather::WeatherData>::iterator entry = entries.begin(); entry != entries.end(); ++entry)
    {
        // update if already existing
        unsigned long pos = findWeatherData(entry->name);
        if (pos < m_WeatherData.size())
            m_WeatherData[pos].value = entry->value;
        // new weather sensor?
        else if (entry->name.startsWith("WEATHER_"))
            m_WeatherData.push_back({QString(entry->name), QString(entry->label), entry->value});
    }
    // update UI
    emit newStatus(status());
}

unsigned long ObservatoryWeatherModel::findWeatherData(const QString name)
{
    unsigned long i;
    for (i = 0; i < m_WeatherData.size(); i++)
    {
        if (m_WeatherData[i].name.compare(name) == 0)
            return i;
    }
    // none found
    return i;
}
} // Ekos

/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2011  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "QuickSettingsWindow.h"

#include "ui_QuickSettingsWindow.h"

#include "QuickSettingsLogger.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/PluginManager.h>

#include <quarkplayer-plugins/MainWindow/MainWindow.h>

#include <TkUtil/LanguageChangeEventFilter.h>
#include <TkUtil/ActionCollection.h>

#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>
#include <phonon/mediasource.h>
#include <phonon/videowidget.h>
#include <phonon/effect.h>
#include <phonon/backendcapabilities.h>
#include <phonon/effectparameter.h>

#include <QtGui/QtGui>

Q_EXPORT_PLUGIN2(QuickSettings, QuickSettingsWindowFactory);

const char * QuickSettingsWindowFactory::PLUGIN_NAME = "QuickSettings";

QStringList QuickSettingsWindowFactory::dependencies() const {
	QStringList tmp;
	tmp += MainWindowFactory::PLUGIN_NAME;
	return tmp;
}

PluginInterface * QuickSettingsWindowFactory::create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const {
	return new QuickSettingsWindow(quarkPlayer, uuid, MainWindowFactory::mainWindow());
}

static const int SLIDER_RANGE = 8;
static const int TICK_INTERVAL = 4;

QuickSettingsWindow::QuickSettingsWindow(QuarkPlayer & quarkPlayer, const QUuid & uuid, QWidget * mainWindow)
	: QDialog(mainWindow),
	PluginInterface(quarkPlayer, uuid) {

	_audioOutput = NULL;
	_mediaObject = NULL;
	_videoWidget = NULL;
	_nextEffect = NULL;
	_ui = NULL;

	connect(Actions["Common.Equalizer"], SIGNAL(triggered()), SLOT(show()));
}

QuickSettingsWindow::~QuickSettingsWindow() {
}

void QuickSettingsWindow::show() {
	setupUi();

	init();

	QDialog::show();
}

void QuickSettingsWindow::setupUi() {
	if (_ui) {
		//Already initialized before
		return;
	}

	_ui = new Ui::QuickSettingsWindow();
	_ui->setupUi(this);

	RETRANSLATE(this);

	//saveButton
	connect(_ui->buttonBox, SIGNAL(accepted()), SLOT(saveSettings()));

	//cancelButton
	connect(_ui->buttonBox, SIGNAL(rejected()), SLOT(restoreSettings()));

	connect(_ui->brightnessSlider, SIGNAL(valueChanged(int)), SLOT(setBrightness(int)));
	connect(_ui->hueSlider, SIGNAL(valueChanged(int)), SLOT(setHue(int)));
	connect(_ui->saturationSlider, SIGNAL(valueChanged(int)), SLOT(setSaturation(int)));
	connect(_ui->contrastSlider , SIGNAL(valueChanged(int)), SLOT(setContrast(int)));
	connect(_ui->aspectRatioCombo, SIGNAL(currentIndexChanged(int)), SLOT(setAspect(int)));
	connect(_ui->scaleModeCombo, SIGNAL(currentIndexChanged(int)), SLOT(setScale(int)));

	connect(_ui->audioEffectButton, SIGNAL(clicked()), SLOT(configureAudioEffect()));

	connect(_ui->audioEffectsCombo, SIGNAL(currentIndexChanged(int)), SLOT(audioEffectChanged()));
}

void QuickSettingsWindow::init() {
	_audioOutput = quarkPlayer().currentAudioOutput();
	_mediaObject = quarkPlayer().currentMediaObject();
	_videoWidget = quarkPlayer().currentVideoWidget();
	_audioOutputPath = quarkPlayer().currentAudioOutputPath();

	if (_videoWidget) {
		_ui->brightnessSlider->setValue(int (_videoWidget->brightness() * SLIDER_RANGE));
		_ui->hueSlider->setValue(int (_videoWidget->hue() * SLIDER_RANGE));
		_ui->saturationSlider->setValue(int (_videoWidget->saturation() * SLIDER_RANGE));
		_ui->contrastSlider->setValue(int (_videoWidget->contrast() * SLIDER_RANGE));
		_ui->aspectRatioCombo->setCurrentIndex(_videoWidget->aspectRatio());
		_ui->scaleModeCombo->setCurrentIndex(_videoWidget->scaleMode());
	}

	_ui->crossFadeSlider->setValue((int) (2 * _mediaObject->transitionTime() / 1000.0f));

	initAudioDevices();
	initAudioEffects();
}

void QuickSettingsWindow::initAudioDevices() {
	//Insert audio devices
	_ui->audioDevicesCombo->clear();
	QList<Phonon::AudioOutputDevice> devices = Phonon::BackendCapabilities::availableAudioOutputDevices();
	for (int i = 0; i < devices.size(); i++) {
		QString itemText = devices[i].name();
		_ui->audioDevicesCombo->addItem(itemText);
		if (devices[i] == _audioOutput->outputDevice()) {
			_ui->audioDevicesCombo->setCurrentIndex(i);
		}
	}
}

void QuickSettingsWindow::initAudioEffects() {
	//Insert audio effects
	_ui->audioEffectsCombo->clear();
	_ui->audioEffectsCombo->addItem(tr("<no effect>"));
	QList<Phonon::Effect *> currentEffects = _audioOutputPath.effects();
	Phonon::Effect * currentEffect = currentEffects.size() ? currentEffects[0] : 0;
	QList<Phonon::EffectDescription> availableEffects = Phonon::BackendCapabilities::availableAudioEffects();
	for (int i = 0; i < availableEffects.size(); i++) {
		_ui->audioEffectsCombo->addItem(availableEffects[i].name());
		if (currentEffect && availableEffects[i] == currentEffect->description()) {
			_ui->audioEffectsCombo->setCurrentIndex(i + 1);
		}
	}
}

void QuickSettingsWindow::saveSettings() {
	_mediaObject->setTransitionTime((int) (1000 * float(_ui->crossFadeSlider->value()) / 2.0f));
	QList<Phonon::AudioOutputDevice> devices = Phonon::BackendCapabilities::availableAudioOutputDevices();

	QList<Phonon::Effect *> currentEffects;
	if (_audioOutput) {
		_audioOutput->setOutputDevice(devices[_ui->audioDevicesCombo->currentIndex()]);
		currentEffects = _audioOutputPath.effects();
	}

	if (_ui->audioEffectsCombo->currentIndex() > 0) {
		Phonon::Effect * currentEffect = currentEffects.size() ? currentEffects[0] : 0;
		if (!currentEffect || currentEffect->description() != _nextEffect->description()) {
			foreach (Phonon::Effect * effect, currentEffects) {
				_audioOutputPath.removeEffect(effect);
				delete effect;
			}
			_audioOutputPath.insertEffect(_nextEffect);
		}
	} else {
		foreach (Phonon::Effect * effect, currentEffects) {
			_audioOutputPath.removeEffect(effect);
			delete effect;
			_nextEffect = NULL;
		}
	}
}

void QuickSettingsWindow::restoreSettings() {
	if (_videoWidget) {
		//TODO Make it dynamic, no restore settings
		float oldBrightness = _videoWidget->brightness();
		float oldHue = _videoWidget->hue();
		float oldSaturation = _videoWidget->saturation();
		float oldContrast = _videoWidget->contrast();
		Phonon::VideoWidget::AspectRatio oldAspect = _videoWidget->aspectRatio();
		Phonon::VideoWidget::ScaleMode oldScale = _videoWidget->scaleMode();
		int currentEffect = _ui->audioEffectsCombo->currentIndex();
		//

		//Restore previous settings
		_videoWidget->setBrightness(oldBrightness);
		_videoWidget->setSaturation(oldSaturation);
		_videoWidget->setHue(oldHue);
		_videoWidget->setContrast(oldContrast);
		_videoWidget->setAspectRatio(oldAspect);
		_videoWidget->setScaleMode(oldScale);
		_ui->audioEffectsCombo->setCurrentIndex(currentEffect);
	}
}

void QuickSettingsWindow::audioEffectChanged() {
	int currentIndex = _ui->audioEffectsCombo->currentIndex();
	if (currentIndex > 0) {
		QList<Phonon::EffectDescription> availableEffects = Phonon::BackendCapabilities::availableAudioEffects();
		Phonon::EffectDescription chosenEffect = availableEffects[currentIndex - 1];

		QList<Phonon::Effect *> currentEffects = _audioOutputPath.effects();
		Phonon::Effect * currentEffect = currentEffects.size() ? currentEffects[0] : 0;

		//Deleting the running effect will stop playback, it is deleted when removed from path
		if (_nextEffect && !(currentEffect && (currentEffect->description().name() == _nextEffect->description().name()))) {
			delete _nextEffect;
		}

		_nextEffect = new Phonon::Effect(chosenEffect);
	}
	_ui->audioEffectButton->setEnabled(currentIndex);
}

void QuickSettingsWindow::updateAudioEffect() {
	for (int i = 0; i < _nextEffect->parameters().size(); i++) {
		Phonon::EffectParameter param = _nextEffect->parameters()[i];

		switch (param.type()) {

		case QVariant::Int: {
			QSpinBox * spin = (QSpinBox *) _propertyControllers.value(param.name());
			_nextEffect->setParameterValue(param, spin->value());
			break;
		}

		case QVariant::Double:
			if (param.minimumValue() == -1.0 && param.maximumValue() == 1.0) {
				QSlider * slider = (QSlider *) _propertyControllers.value(param.name());
				_nextEffect->setParameterValue(param, (double) (slider->value() / (double) SLIDER_RANGE));
			} else {
				QDoubleSpinBox * spin = (QDoubleSpinBox *) _propertyControllers.value(param.name());
				_nextEffect->setParameterValue(param, spin->value());
			}
			break;

		case QVariant::Bool: {
			QCheckBox * checkBox = (QCheckBox *) _propertyControllers.value(param.name());
			_nextEffect->setParameterValue(param, checkBox->isChecked());
			break;
		}

		case QVariant::String: {
			QLineEdit * edit = (QLineEdit *) _propertyControllers.value(param.name());
			_nextEffect->setParameterValue(param, edit->text());
			break;
		}

		default:
			break;
		}
	}
}

void QuickSettingsWindow::configureAudioEffect() {
	int currentIndex = _ui->audioEffectsCombo->currentIndex();
	if (currentIndex > 0) {
		QList<Phonon::EffectDescription> availableEffects = Phonon::BackendCapabilities::availableAudioEffects();
		Phonon::EffectDescription chosenEffect = availableEffects[currentIndex - 1];

		QDialog effectDialog(this);
		effectDialog.setWindowTitle(tr("Configure effect"));
		QVBoxLayout * topLayout = new QVBoxLayout(&effectDialog);

		QLabel * description = new QLabel("<b>" + tr("Description") + ":</b><br>" + chosenEffect.description(), &effectDialog);
		description->setWordWrap(true);
		topLayout->addWidget(description);

		QScrollArea * scrollArea = new QScrollArea(&effectDialog);
		topLayout->addWidget(scrollArea);

		QWidget * scrollWidget = new QWidget(&effectDialog);
		QVBoxLayout * scrollLayout = new QVBoxLayout(scrollWidget);
		scrollWidget->setMinimumWidth(320);
		scrollArea->setWidget(scrollWidget);

		if (_nextEffect) {
			foreach (Phonon::EffectParameter param, _nextEffect->parameters()) {
				QHBoxLayout * hLayout = new QHBoxLayout();
				QString labelName = param.name();
				labelName[0] = labelName[0].toUpper();
				hLayout->addWidget(new QLabel("<b>" + labelName + ":</b>"));

				if (param.type() == QVariant::Int) {
					QSpinBox * spin = new QSpinBox(&effectDialog);
					spin->setMinimum(param.minimumValue().toInt());
					spin->setMaximum(param.maximumValue().toInt());
					QVariant currentValue = _nextEffect->parameterValue(param);
					spin->setProperty("oldValue", currentValue.toInt());
					spin->setValue(currentValue.toInt());
					connect(spin, SIGNAL(valueChanged(int)), SLOT(updateEffect()));
					hLayout->addWidget(spin);
					_propertyControllers.insert(param.name(), spin);
				}

				else if (param.type() == QVariant::Double) {
					if (param.minimumValue() == -1.0 && param.maximumValue() == 1.0) {
						//Special case values between -1 and 1.0 to use a slider for improved usability
						QSlider * slider = new QSlider(Qt::Horizontal, &effectDialog);
						slider->setMinimum(-SLIDER_RANGE);
						slider->setMaximum(SLIDER_RANGE);
						QVariant currentValue = _nextEffect->parameterValue(param);
						slider->setProperty("oldValue", currentValue.toDouble());
						slider->setValue((int) (SLIDER_RANGE * currentValue.toDouble()));
						slider->setTickPosition(QSlider::TicksBelow);
						slider->setTickInterval(TICK_INTERVAL);
						hLayout->addWidget(slider);
						connect(slider, SIGNAL(valueChanged(int)), SLOT(updateEffect()));
						_propertyControllers.insert(param.name(), slider);
					} else {
						QDoubleSpinBox * spin = new QDoubleSpinBox(&effectDialog);
						spin->setSingleStep(0.1);
						spin->setMinimum(param.minimumValue().toDouble());
						spin->setMaximum(param.maximumValue().toDouble());
						QVariant currentValue = _nextEffect->parameterValue(param);
						spin->setProperty("oldValue", currentValue);
						spin->setValue(currentValue.toDouble());
						connect(spin, SIGNAL(valueChanged(double)), SLOT(updateEffect()));
						hLayout->addWidget(spin);
						_propertyControllers.insert(param.name(), spin);
					}
				}

				else if (param.type() == QVariant::Bool) {
					QCheckBox * checkBox = new QCheckBox(&effectDialog);
					QVariant currentValue = _nextEffect->parameterValue(param);
					checkBox->setProperty("oldValue", currentValue);
					checkBox->setChecked(currentValue.toBool());
					connect(checkBox, SIGNAL(stateChanged(int)), SLOT(updateEffect()));
					hLayout->addWidget(checkBox);
					_propertyControllers.insert(param.name(), checkBox);
				}

				else if (param.type() == QVariant::String) {
					QLineEdit * edit = new QLineEdit(&effectDialog);
					QVariant currentValue = _nextEffect->parameterValue(param);
					edit->setProperty("oldValue", currentValue.toString());
					edit->setText(currentValue.toString());
					connect(edit, SIGNAL(returnPressed()), this, SLOT(updateEffect()));
					hLayout->addWidget(edit);
					_propertyControllers.insert(param.name(), edit);
				}
				scrollLayout->addLayout(hLayout);
			}

			QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &effectDialog);
			connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), &effectDialog, SLOT(accept()));
			connect(buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), &effectDialog, SLOT(reject()));
			topLayout->addWidget(buttonBox);

			scrollWidget->adjustSize();
			effectDialog.adjustSize();

			effectDialog.exec();
			if (effectDialog.result() != QDialog::Accepted) {
				//Revert any changes
				foreach (Phonon::EffectParameter param, _nextEffect->parameters()) {

					switch (param.type()) {

					case QVariant::Int: {
						QSpinBox * spin = (QSpinBox *) _propertyControllers.value(param.name());
						_nextEffect->setParameterValue(param, spin->property("oldValue").toInt());
						break;
					}

					case QVariant::Double:
						if (param.minimumValue() == -1.0 && param.maximumValue() == 1.0) {
							QSlider * slider = (QSlider *) _propertyControllers.value(param.name());
							_nextEffect->setParameterValue(param, slider->property("oldValue").toDouble());
						} else {
							QDoubleSpinBox * spin = (QDoubleSpinBox *) _propertyControllers.value(param.name());
							_nextEffect->setParameterValue(param, spin->property("oldValue").toDouble());
						}
						break;

					case QVariant::Bool: {
						QCheckBox * checkBox = (QCheckBox *) _propertyControllers.value(param.name());
						_nextEffect->setParameterValue(param, checkBox->property("oldValue").toBool());
						break;
					}

					case QVariant::String: {
						QLineEdit * edit = (QLineEdit *) _propertyControllers.value(param.name());
						_nextEffect->setParameterValue(param, edit->property("oldValue").toString());
						break;
					}

					default:
						break;
					}
				}
			}
			_propertyControllers.clear();
		}
	}
}

void QuickSettingsWindow::setSaturation(int value) {
	if (_videoWidget) {
		_videoWidget->setSaturation(value / qreal(SLIDER_RANGE));
	}
}

void QuickSettingsWindow::setHue(int value) {
	if (_videoWidget) {
		_videoWidget->setHue(value / qreal(SLIDER_RANGE));
	}
}

void QuickSettingsWindow::setAspect(int value) {
	if (_videoWidget) {
		_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatio(value));
	}
}

void QuickSettingsWindow::setScale(int value) {
	if (_videoWidget) {
		_videoWidget->setScaleMode(Phonon::VideoWidget::ScaleMode(value));
	}
}

void QuickSettingsWindow::setBrightness(int value) {
	if (_videoWidget) {
		_videoWidget->setBrightness(value / qreal(SLIDER_RANGE));
	}
}

void QuickSettingsWindow::setContrast(int value) {
	if (_videoWidget) {
		_videoWidget->setContrast(value / qreal(SLIDER_RANGE));
	}
}

void QuickSettingsWindow::retranslate() {
	_ui->retranslateUi(this);
}

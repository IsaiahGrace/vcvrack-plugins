#include "plugin.hpp"


struct Mux1 : Module {
	enum ParamId {
		PARAMS_LEN
	};
	enum InputId {
		SEL0_INPUT,
		IN0_INPUT,
		IN1_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		SEL_LED_LIGHT,
		IN0_LED_LIGHT,
		OUT_LED_LIGHT,
		IN1_LED_LIGHT,
		LIGHTS_LEN
	};

	dsp::ClockDivider lightDivider;
	dsp::SlewLimiter clickFilter;

	bool declick = false;

	Mux1() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(SEL0_INPUT, "");
		configInput(IN0_INPUT, "");
		configInput(IN1_INPUT, "");
		configOutput(OUT_OUTPUT, "");

		clickFilter.rise = 400.f; // Hz
		clickFilter.fall = 400.f; // Hz

		lightDivider.setDivision(32);
	}

	void onReset(const ResetEvent& e) override {
		Module::onReset(e);
		declick = false;
	}

	void process(const ProcessArgs& args) override {
		bool sel = inputs[SEL0_INPUT].getVoltage() >= 1.f;
		float in0 = inputs[IN0_INPUT].getVoltage();
		float in1 = inputs[IN1_INPUT].getVoltage();

		if (declick) {
			float gain = clickFilter.process(args.sampleTime, sel);
			float voltage = (in1 * gain) + (in0 * (1.f - gain));
			outputs[OUT_OUTPUT].setVoltage(voltage);
		} else {
			if (sel) {
				outputs[OUT_OUTPUT].setVoltage(in1);
			} else {
				outputs[OUT_OUTPUT].setVoltage(in0);
			}
		}

		if (lightDivider.process()) {
			float lightTime = args.sampleTime * lightDivider.getDivision();
			lights[IN0_LED_LIGHT].setBrightnessSmooth(in0, lightTime);
			lights[IN1_LED_LIGHT].setBrightnessSmooth(in1, lightTime);
			if (sel) {
				lights[SEL_LED_LIGHT].setBrightness(1.f);
				lights[OUT_LED_LIGHT].setBrightnessSmooth(in1, lightTime);
			} else {
				lights[SEL_LED_LIGHT].setBrightness(0.f);
				lights[OUT_LED_LIGHT].setBrightnessSmooth(in0, lightTime);
			}
		}
	}

	json_t* dataToJson() override {
		json_t* rootJ = json_object();
		json_object_set_new(rootJ, "declick", json_boolean(declick));
		return rootJ;
	}

	void dataFromJson(json_t* rootJ) override {
		json_t* declickJ = json_object_get(rootJ, "declick");
		declick = json_boolean_value(declickJ);
	}
};


struct Mux1Widget : ModuleWidget {
	Mux1Widget(Mux1* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/mux1.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.4, 35.56)), module, Mux1::SEL0_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.16, 60.96)), module, Mux1::IN0_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.16, 81.28)), module, Mux1::IN1_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(40.64, 71.12)), module, Mux1::OUT_OUTPUT));

		addChild(createLightCentered<SmallLight<WhiteLight>>(mm2px(Vec(25.4, 43.18)), module, Mux1::SEL_LED_LIGHT));
		addChild(createLightCentered<SmallLight<GreenLight>>(mm2px(Vec(17.78, 60.96)), module, Mux1::IN0_LED_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(33.02, 71.12)), module, Mux1::OUT_LED_LIGHT));
		addChild(createLightCentered<SmallLight<GreenLight>>(mm2px(Vec(17.78, 81.28)), module, Mux1::IN1_LED_LIGHT));
	}

	void appendContextMenu(Menu* menu) override {
		Mux1* module = getModule<Mux1>();

		menu->addChild(new MenuSeparator);

		menu->addChild(createBoolPtrMenuItem("De-click", "", &module->declick));
	}
};


Model* modelMux1 = createModel<Mux1, Mux1Widget>("mux1");

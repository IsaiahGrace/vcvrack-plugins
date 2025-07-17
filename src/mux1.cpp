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

	Mux1() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(SEL0_INPUT, "");
		configInput(IN0_INPUT, "");
		configInput(IN1_INPUT, "");
		configOutput(OUT_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
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

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(25.4, 43.18)), module, Mux1::SEL_LED_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(17.78, 60.96)), module, Mux1::IN0_LED_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(33.02, 71.12)), module, Mux1::OUT_LED_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(17.78, 81.28)), module, Mux1::IN1_LED_LIGHT));
	}
};


Model* modelMux1 = createModel<Mux1, Mux1Widget>("mux1");

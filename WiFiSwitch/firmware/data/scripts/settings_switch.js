import { ParametersPageController } from "./controllers/page_controller.js";
import { SliderController } from "./controllers/slider_controller.js"
import { ComboBoxController } from "./controllers/combobox_controller.js"
import { MultiStateImage } from "./controllers/multi_state_image.js"
import { HttpProcessor } from "./http_processor.js";

class SwitchSettingsPageController extends ParametersPageController{
    constructor(theEl){
        super(theEl);
    }

    async _upadteAfterLoad(){
        super._upadteAfterLoad();
        this._m_BlockUpdates = false;
        let anOnSlEl = this._m_BaseElement.querySelector("#on_brigness_slider");
        this._m_OnSlider = new SliderController(anOnSlEl);
        this._m_OnSlider.setChangeCallback(this._onSettChanged.bind(this));
        let anOffSlEl = this._m_BaseElement.querySelector("#off_brigness_slider");
        this._m_OffSlider = new SliderController(anOffSlEl);
        this._m_OffSlider.setChangeCallback(this._onSettChanged.bind(this));
        let aSoundEl = this._m_BaseElement.querySelector("#sound_image");
        let aStates = ["images/sound_on.png", "images/sound_off.png"];
        this._m_Sound = new MultiStateImage(aSoundEl, aStates);
        this._m_Sound.setChangeCallback(this._onSettChanged.bind(this));
        let aStyleEl = this._m_BaseElement.querySelector("#switch_style");
        this._m_StyleCombo = new ComboBoxController(aStyleEl);
        this._m_StyleCombo.setChangeCallback(this._onSettChanged.bind(this));
    }

    async _onSettChanged(){
        if( this._m_BlockUpdates )
          return;
        let anOnVal = this._m_OnSlider.getValue();
        let anOffVal = this._m_OffSlider.getValue();
        let aStyle = this._m_StyleCombo.getCurrentItem();
        let aSound = this._m_Sound.getState();
        let aData = new Object();
        aData.on_brightness = anOnVal;
        aData.off_brightness = anOffVal;
        aData.style = aStyle;
        aData.sound = "off";
        if( aSound == 0 ){
            aData.sound = "on";
        }
        await HttpProcessor.sendPostJSON(this._m_ParamReq, aData);
    }

    async setPageParameters(theData){
        this._m_BlockUpdates = true;
        this._m_OnSlider.setValue(theData.on_brightness);
        this._m_OffSlider.setValue(theData.off_brightness);
        if( theData.sound ){
            if(theData.sound == "on"){
                this._m_Sound.setState(0);
            }
            else{
                this._m_Sound.setState(1);
            }
        }
        this._m_StyleCombo.setCurrentItem(theData.style);
        this._m_BlockUpdates = false;
    }

};

export {SwitchSettingsPageController};
import {FormPageController} from "./controllers/page_controller.js"
import {PasswordController} from "./controllers/password_controller.js"
import {EnableController} from "./controllers/enable_controller.js";

class MqttSettingsPageController extends FormPageController{
    constructor(theEl){
        super(theEl);
    }


    async setPageParameters(theInfo){
        let aMqttRadio = this._m_BaseElement.querySelector("#is_mqtt");
        aMqttRadio.onchange = this._updateInterfaces.bind(this); 

        super.setPageParameters(theInfo)
        let aPwds = this._m_BaseElement.querySelectorAll('[data-controller_class="PasswordController"]');
        for( let i=0; i< aPwds.length ; i++ ){
            let anEl = aPwds[i];
            let aController = new PasswordController(anEl); 
        }
        this._updateInterfaces()
    }

    _updateInterfaces(){
        let aMqttCheckBox = this._m_BaseElement.querySelector("#is_mqtt");
        let aMqttGroup = this._m_BaseElement.querySelector("#mqtt_group")
        if( aMqttCheckBox.checked){
            aMqttGroup.className="active_properties_group";
        }
        else{
            aMqttGroup.className="properties_group";
        }
    }
}


export {MqttSettingsPageController};
import {FormPageController} from "./controllers/page_controller.js"
import {PasswordController} from "./controllers/password_controller.js"
import {EnableController} from "./controllers/enable_controller.js";

class WiFiSettingsPageController extends FormPageController{
    constructor(theEl){
        super(theEl);
        this._m_IsSingleInt = true;
    }


    async setPageParameters(theInfo){
        let anAPRadio = this._m_BaseElement.querySelector("#access_point_type");
        anAPRadio.onchange = this._onAPType.bind(this); 
        let aStRadio = this._m_BaseElement.querySelector("#station_type");
        aStRadio.onchange = this._onSTType.bind(this); 
        let aPwds = this._m_BaseElement.querySelectorAll('[data-controller_class="PasswordController"]');
        for( let i=0; i< aPwds.length ; i++ ){
            let anEl = aPwds[i];
            let aController = new PasswordController(anEl); 
        }
        let aStIsFixedIPEl = this._m_BaseElement.querySelector("#st_fixed_ip");
        let aStIp = this._m_BaseElement.querySelector("#st_ip");
        let aStNet = this._m_BaseElement.querySelector("#st_netmask");
        let aStGw = this._m_BaseElement.querySelector("#st_gateway");
        let aStElArray = [];
        aStElArray.push(aStIp);
        aStElArray.push(aStNet);
        aStElArray.push(aStGw);
        this._m_FixedStIp = new EnableController(aStIsFixedIPEl,aStElArray);

        let anAPIsFixedIPEl = this._m_BaseElement.querySelector("#ap_fixed_ip");
        let anAPIp = this._m_BaseElement.querySelector("#ap_ip");
        let anAPNet = this._m_BaseElement.querySelector("#ap_netmask");
        let anAPGw = this._m_BaseElement.querySelector("#ap_gateway");
        let anAPElArray = [];
        anAPElArray.push(anAPIp);
        anAPElArray.push(anAPNet);
        anAPElArray.push(anAPGw);
        this._m_FixedAPIp = new EnableController(anAPIsFixedIPEl,anAPElArray);
        await super.setPageParameters(theInfo);
        if( this._m_IsSingleInt ){
            let anAPCheckBox = this._m_BaseElement.querySelector("#access_point_type");
            let aSTCheckBox = this._m_BaseElement.querySelector("#station_type");
            if( anAPCheckBox.checked == aSTCheckBox.checked ){
                anAPCheckBox.checked = false;
                aSTCheckBox.checked = true;
            }
        }
        this._updateInterfaces();
        this._m_FixedStIp.update();
        this._m_FixedAPIp.update();
    }

    _onAPType(){
        if( this._m_IsSingleInt ){
            let aAPCheckBox = this._m_BaseElement.querySelector("#access_point_type");
            let aSTCheckBox = this._m_BaseElement.querySelector("#station_type");
            aSTCheckBox.checked = !aAPCheckBox.checked;
        }
        this._updateInterfaces();
    }

    _onSTType(){
        if( this._m_IsSingleInt ){
            let anAPCheckBox = this._m_BaseElement.querySelector("#access_point_type");
            let aSTCheckBox = this._m_BaseElement.querySelector("#station_type");
            anAPCheckBox.checked = !aSTCheckBox.checked;
        }
        this._updateInterfaces();
    }

    _updateInterfaces(){
        let aAPCheckBox = this._m_BaseElement.querySelector("#access_point_type");
        let aSTCheckBox = this._m_BaseElement.querySelector("#station_type");
        let aAPGroup = this._m_BaseElement.querySelector("#ap_group");
        let aSTGroup = this._m_BaseElement.querySelector("#st_group");
        if( aAPCheckBox.checked){
            aAPGroup.className="active_properties_group";
        }
        else{
            aAPGroup.className="properties_group";
        }
        if( aSTCheckBox.checked){
            aSTGroup.className="active_properties_group";
        }
        else{
            aSTGroup.className="properties_group";
        }
    }

/*    async setPageParameters(theData){
        super.setPageParameters(theData);
        if( this._m_IsSingleInt ){
            let anAPCheckBox = this._m_BaseElement.querySelector("#access_point_type");
            let aSTCheckBox = this._m_BaseElement.querySelector("#station_type");
            if( anAPCheckBox.checked == aSTCheckBox.checked ){
                anAPCheckBox.checked = false;
                aSTCheckBox.checked = true;
            }
        }
        this._updateInterfaces();
    }*/
}

export {WiFiSettingsPageController};
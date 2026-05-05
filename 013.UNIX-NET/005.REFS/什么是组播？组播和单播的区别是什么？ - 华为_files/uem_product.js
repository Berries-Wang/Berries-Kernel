var env = 'pro';
//         env 代表埋码环境，值只能是: beta 或 pro
//         env = "beta";   // 代表beta环境
//         env = "pro";    // 代表生产环境
// 埋码AppKey配置
var hwaAppKeys = {
  'env' : env,
  'appKeys' : {
    'beta' : '', //从UEM的beta环境获取的埋码Key
    'pro' : 'f943da07953b09ca52368c7dbc843696' 	  //从UEM的生产环境获取的埋码Key
  },
  'enable': true,  		//关闭UEM采集开关(可选,默认是开启的)
  'channel': '',    		//埋码的频道标识符(可选),比如天幕平台埋码:DMAX
  'ABVersion': 'A',    	//AB版本标识(可选),取值 A或者B;A代表A版本,B代表B版本
  'platform': 'web'    	//埋码应用的运行平台(可选,默认是web), PC页面:web;we码程序:welink
};
// 采集器配置
;(function(h, w, f, t) {
  try {
    if (!h[t]) {
      h.GlobalHwaNamespace = h.GlobalHwaNamespace || [];
      h.GlobalHwaNamespace.push(t);
      h[t] = function() {
        (h[t].q = h[t].q || []).push(arguments);
      };
      h['trackerload'] = function() {
        (h[t].q = h[t].q && h[t].q.length ? h[t].q : []).unshift(arguments);
      };
      h[t].q = h[t].q || [];
    }
    var host = ('pro' == f['env']) ? 'hwa.his.huawei.com' : (('beta' == f['env']) ? 'hwa-beta.his.huawei.com' : '');
    var welinkHost = '';
    if ('welink' == f['platform']) {
      host = (('pro' == f['env']) ? 'w3m.huawei.com/mcloud/mag' : (('beta' == f['env']) ? 'mcloud-uat.huawei.com/mcloud/mag' : ''));
      welinkHost = host + '/ProxyForText/hwa_trackload';
      h.hwahost = welinkHost;
      host = host + '/fg/ProxyForDownLoad/hwa_f';
    }
    h.aids = f;
    h.space = t;
    var iframe = w.createElement('iframe');
    (iframe.frameElement || iframe).style.cssText = 'display:none';
    iframe.src = 'javascript:false';
    iframe.id = 'hwa_f';
    var where = w.getElementsByTagName('script')[0];
    where.parentNode.insertBefore(iframe, where);
    var doc = iframe.contentWindow.document;
    var async = !(f['async'] == false) ? 'js.async=1;js.defer=\'defer\'' : '';
    var ts = new Date().getFullYear() + '' + new Date().getMonth() + '' + new Date().getDate();
    doc.open().write('<body onload="var js = document.createElement(\'script\'); ' + async + ';js.src = \'' + ("https:" == location.protocol ? "https://" : "http://") + host + '/dist/hwa_f.js?v='+ (ts) +'\';document.body.appendChild(js);">');
    doc.close();
    iframe.contentWindow.aids = f;
    iframe.contentWindow.space = t;
    iframe.contentWindow.hwahost = welinkHost;
  } catch (e) {
    if (!h[t]) h[t] = function(){};
    
  }
})(window, document, hwaAppKeys, 'hwa');
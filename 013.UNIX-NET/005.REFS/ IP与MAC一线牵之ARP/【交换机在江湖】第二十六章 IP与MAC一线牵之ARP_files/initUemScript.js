;(function () {
  //  此时拿不到 tenetId，只能通过域名判断（切换域名后需要修改此处代码）
  const isO3Page = window.location.host.includes('beta.')
  const env = ('prod' || 'dev') == 'prod' ? 'prod' : 'beta'

  const configs = {
    beta: {
      platform: 'web', // 浏览器"web" || WE码小程序"welink" || WE码H5轻应用"welinkh5"
      edition: 'his', // 红版"his" || 蓝版"heds"
      env: 'beta', // 测试"beta" || 生产"prod"
      appKey: '6b6f02a8943a0d222d3892451df0719e', // 租户的UEM埋码key
      src: 'https://hwa-beta.his.huawei.com/dist/uem_f.js' // 详见最下方的注释: @desc src属性不同环境的对应值
    },
    prod: {
      platform: 'web', // 浏览器"web" || WE码小程序"welink" || WE码H5轻应用"welinkh5"
      edition: 'his', // 红版"his" || 蓝版"heds"
      env: 'prod', // 测试"beta" || 生产"prod"
      appKey: 'f1853fbe41198c76ed44b39f361eb7a9', // 租户的UEM埋码key
      src: 'https://hwa.his.huawei.com/dist/uem_f.js' // 详见最下方的注释: @desc src属性不同环境的对应值
    }
  }

  // 初始化UEM采集器
  ;(function (config) {
    var namespace = config.namespace || 'hwa'
    if (self[namespace] || document.querySelector('script#uem_f')) {
      return
    }
    self[namespace] = function () {
      ;(self[namespace].q = self[namespace].q || []).push(arguments)
    }
    var element = document.createElement('script')
    element.TRACKER_CONFIG = config
    element.async = true
    element.id = 'uem_f'
    element.src = config.src + '?_' + new Date().toJSON().split('T')[0]
    document.head.appendChild(element)
  })(configs[env])
})()

//复制过来的意见反馈代码
//登录的相关属性
var queryParam = {
    language : null,
    loginPreURL :"",
    isLogin : null,
    requestURL : "",
    lang:$('#lang').val()
}


$(function(){
    // 初始化页面时就获取用户是否登录
    //加载对话框
    loadDialog();
    //点击意见反馈按钮，详情页和切换页签的点击事件
    $("#feedback,#feedback2").off("click").on("click",function(event){
        stopBubble(event);
        // 没登陆跳转登陆界面
        if(JSON.parse($('#isLogin').val())){
            //设置该页面的pageId
            setPageId();
            // 加载意见反馈弹出框的内容
            loadFeedBackIframe("#doc_feedback","id_frm_docfeedback", "setCustomerFeedbackHeight");
            //加载意见反馈弹窗
            loadFeedbackButton();
            $('#autoReply').siblings('.ui-dialog').css('top','250px');
        }else{
            // 登陆
            forLogin();
        }
    });
});

function getUserInfo()
{
    // 如果本地调试页头，要将这三行注释掉，不然会不走自己的断点。



    var comPath = $('#common-chang-path').val();
    $.ajax({
        type : "post",
        url : comPath+"/user/getHeadInfo",
        dataType : "json",
        cache : false,
        async : false,
        success : function(data){
            queryParam.language = 'en';
            queryParam.loginPreURL = HTMLDecode(data.loginPreURL);
            queryParam.isLogin = data.isLogin;
        },
        error: function(){}
    });
}

//将字符串转HTML
function HTMLDecode(value) {
    try {
        value = value.replace(/</g,"&lt;");
        value = value.replace(/>/g,"&gt;");
        var temp = document.createElement("div");
        temp.innerHTML = value;
        var output = temp.innerText || temp.textContent;
        if(output == "undefined") {
            output = "";
        }
        temp = null;
        return output;
    }catch (e) {
        return value;
    }
}
/**
 * 判断浏览器是PC端/移动端
 * @returns {boolean} true 移动端/false PC端
 * @since 2020年10月17日19点40分
 */
function isMobile() {
    if (navigator.userAgent.match(/(phone|pod|iPhone|iPod|ios|Android|BlackBerry|IEMobile|MQQBrowser|JUC|Fennec|wOSBrowser|BrowserNG|WebOS|Symbian|Windows Phone)/i) || window.innerWidth <= 768) {
        return true;
    }
    return false;
}
/**
 * 加载对话框
 */
function loadDialog(){
    var windowWidth = window.innerWidth || document.documentElement.clientWidth || document.body.clientWidth;
    var windowHeight = window.innerHeight || document.documentElement.clientHeight || document.body.clientHeight;
    var feedbackWidth = windowWidth;
    var headerDIVHeight = $("#headerDIV").height()+$('#headerTop').height();
    var feedbackHeight = windowHeight - 2 * headerDIVHeight;// 解决页头把意见反馈dialog的头部挡住了
    if(!isMobile()){
        feedbackWidth = windowWidth * 1000 / 1680;
        if(feedbackWidth > 1000)
        {
            feedbackWidth = 1000;
        }
        if(feedbackWidth < 850)
        {
            feedbackWidth = 850;
        }
        feedbackHeight = windowHeight * 540 / 950;
        if(feedbackHeight > 540)
        {
            feedbackHeight = 540;
        }
    }

    var feedbackTitle = "意见反馈";
    var lang = $("#lang").val();
    if("zh" != lang)
    {
        feedbackTitle = "Feedback";
    }
    $("#doc_feedback").dialog({
        autoOpen : false,
        title : feedbackTitle,
        resizable : false,
        width : feedbackWidth,
        height : feedbackHeight,
        modal : true,
        zIndex: 200,
        close : function() {
            $("#doc_feedback").dialog("close");
        }
    });
    $(".ui-dialog .ui-dialog-titlebar .ui-dialog-titlebar-close").append(checkXssHTML("<span class='ui-button-icon ui-icon ui-icon-closethick'></span><span class='ui-button-icon-space'></span>"));
    $(".ui-dialog .ui-dialog-titlebar .ui-dialog-titlebar-close .ui-button-icon").css("position","absolute");
    $(".ui-dialog .ui-dialog-titlebar .ui-dialog-titlebar-close .ui-button-icon").css("top","50%");
    $(".ui-dialog .ui-dialog-titlebar .ui-dialog-titlebar-close .ui-button-icon").css("left","50%");
    $(".ui-dialog .ui-dialog-titlebar .ui-dialog-titlebar-close .ui-button-icon").css("margin-top","-8px");
    $(".ui-dialog .ui-dialog-titlebar .ui-dialog-titlebar-close .ui-button-icon").css("margin-left","-8px");
}

/**
 * 意见反馈弹出显示
 */
function loadFeedbackButton() {
    $("#dialog:ui-dialog").dialog("destroy");
    $("#doc_feedback").dialog("open");
    if(isMobile()){
        $("#doc_feedback").css({
            "overflow":"hidden",
            "overflow-y":"auto",
            "max-height":"600px",
            "height":"auto"
        });
    }
    $("div[aria-describedby='doc_feedback'] .ui-dialog-title").css("text-align","left");
}

function setCustomerFeedbackHeight(heights) {
    document.getElementById('id_frm_docfeedback').style.height = heights + 'px';
}

function getSafeLink(link) {
    if (link && link.indexOf('http') != 0 && link.indexOf('/') != 0) {
        return "javascript:void(0);";
    }
    return link;
}
function getFeedbackTopicId(){
    var pageId = $('#page_id').val();
    return pageId;
}
var pcDetail={
    isExist : null
}
$(function () {
    const searchWord = $('#entityInfo_name').val();
    const entityInfo_encodeName = $('#entityInfo_encodeName').val();
    //1.将请求controller的路径换成按规则处理成静态html的url; 2.针对词条名称含有/特殊字符的前端转成 ## 后端在进行处理,2021/7/27 将/处理成-
    const url = location.href.replace(/detail\?.+/, entityInfo_encodeName + '.html');
    history.replaceState('', '', url);
    const $input = $('.input-box input');
    let isExist = false;
    const lang = $('#lang').val();
    const domain = $('#domain').val();
    // 搜索
    initSearchInputDetail(lang);

    $input.focus(function () {
        $('.input-box').css('border', '1px solid #333');
        var searchWord = $.trim($('#search-inputbox').val());
        searchWord=replaceSpecialCharacters(searchWord);
        if(searchWord && searchWord.length>1){
            $('#search-count #ui-id-1').show();
        }
    });
    $input.blur(function () {
        $('.input-box').css('border', '');
    });
    $input.on('input', function () {
        if ($(this).val()) {
            $('.baike-remove').show();
        } else {
            pcDetail.isExist = false;
            $('.baike-remove').hide();
        }
    });
    $input.on('keydown', function (e) {
        if (e.keyCode == 13)
            $('.baike-search').click();
    });

    $('.suggest').on('click', 'li', function () {
        var searchWord = $(this).text().trim();
        searchWord=replaceSpecialCharacters(searchWord);
        sessionStorage.setItem('searchWord', searchWord);
        sessionStorage.setItem('isExactSearch', 'true');
        location.href = $("#contextPath").val() + '/encyclopedia/' + lang + '/detail?action=queryEntityDetail&keyword=' + encodeURI(searchWord);
    });
    $('.baike-remove').on('click', function () {
        $input.val('');
        $(this).hide();
    });
    $('.baike-search').hover(function () {
        $(this).attr('src', $("#contextPath").val() + '/pages/EncyclopediaJsp/encyclopaedia/img/search_hover.png')
    }, function () {
        $(this).attr('src', $("#contextPath").val() + '/pages/EncyclopediaJsp/encyclopaedia/img/search.png')
    });
    $('.baike-search').click(function () {
        var searchWord = $input.val().trim().trim();
        if(filterXssString(searchWord)){
            return -1;
        }
        if (searchWord) {
            searchWord=replaceSpecialCharacters(searchWord);
            if (pcDetail.isExist) {
                sessionStorage.setItem('searchWord', searchWord);
                sessionStorage.setItem('isExactSearch', 'false');
                location.href = $("#contextPath").val() + '/encyclopedia/' + lang + '/detail?action=queryMatchedEntityDetail&keyword=' + encodeURI(searchWord);
            } else if(pcDetail.isExist == null) {
                sessionStorage.setItem('searchWord', searchWord);
                sessionStorage.setItem('isExactSearch', 'false');
                //为模糊搜索
                location.href = $("#contextPath").val() + '/encyclopedia/' + lang + '/detail?action=queryMatchedEntityDetail&keyword=' + encodeURI(searchWord);
            }else{
                location.href = $("#contextPath").val() + '/encyclopedia/' + lang + '/error';
            }
        }
    });

    let pageTitle = $('#entityInfo_webPageTitle').val().replace('- Huawei', '').replace('- 华为', '');
    let shareImage = $('#entityInfo_image').val().split('src="')[1].split('"')[0];


    // 目录展开收起
    if (lang === 'en') {
        if ($('.catalogue-right ul li').length > 4) {
            $('.more').show();
        }
    } else {
        if ($('.catalogue-right ul li').length > 6) {
            $('.more').show();
        }
    }

    $('.more>div').click(function () {
        $(this).hide().siblings().show();
        if ($(this).find('span').text().trim() === '更多' || $(this).find('span').text().trim() === 'More') {
            $('.catalogue-right ul').css('height', 'auto')
        } else {
            $('.catalogue-right ul').css('height', '72px')
        }
    });
    $('#footerDIV').css('position', 'relative')
    //滚动事件

    const arr = [];
    $('.idp-ltr-html-topictitle2').each(function () {
        arr.push($(this).offset().top);
    });
    const relatedToTop = $('.item-box').offset().top;
    const height = document.documentElement.clientHeight || document.body.clientHeight;
    let footerDIVTotop = $('#footerDIV').offset().top;
    $(window).on('scroll', function () {
        if ($(this).scrollTop() >= relatedToTop) {
            $('.navagation').show();
            $('.item-box').css({top: 75, position: 'fixed'});
        } else {
            $('.navagation').hide();
            $('.item-box').css({top: 0, position: 'absolute'});
        }
        for (let i = 0; i < arr.length; i++) {
            if (i === arr.length - 1) {
                if ($(this).scrollTop() >= arr[i] - 10) {
                    $('.navagation-list img').eq(i).attr('src', `${$("#contextPath").val()}/pages/EncyclopediaJsp/result/img/navarrow.png`)
                        .parent().parent().siblings().find('img').attr('src', `${$("#contextPath").val()}/pages/EncyclopediaJsp/result/img/navround.png`);
                    $('.navagation-list a').eq(i).addClass('checked').parent().siblings().find('a').removeClass('checked');
                }
            } else {
                if ($(this).scrollTop() >= arr[i] - 10 && $(this).scrollTop() < arr[i + 1]) {
                    $('.navagation-list img').eq(i).attr('src', `${$("#contextPath").val()}/pages/EncyclopediaJsp/result/img/navarrow.png`)
                        .parent().parent().siblings().find('img').attr('src', `${$("#contextPath").val()}/pages/EncyclopediaJsp/result/img/navround.png`);
                    $('.navagation-list a').eq(i).addClass('checked').parent().siblings().find('a').removeClass('checked');
                }
            }
        }
    });
    // 右上角视频
    if ($('#entityInfo_video').val()) {
        $('.video').children().not('.masking').css({width: '100%', height: '100%'});
    } else {
        $('.video').hide();
    }

    // 弹出视频播放,周剑毫要求修改 2021/07/27
    if ($('div.idp-ltr-html-screen')) {
        $('div.idp-ltr-html-screen').css('position', 'relative')
        $('div.idp-ltr-html-screen').append(checkXssHTML(`<div class="masking"></div>`))
        $('div.idp-ltr-html-screen .masking').css({
            width: $('div.idp-ltr-html-screen').children().not('.masking').width() + 'px',
            height: $('div.idp-ltr-html-screen').children().not('.masking').height() + 'px',
            left: '0',
            top: '0',
        })
    }
    const w = $('.playvideo').width();
    $('.playvideo').height(0.563 * w);
    $('.masking').click(function () {
        $('.playvideo').show();
        if ($('.playvideo').children().length > 1) {
            $('.playvideo').children().last().remove();
        }
        $('.playvideo').append($(this).siblings().clone().css({width: '100%', height: '100%'}));
    });

    $('.close').click(function () {
        $('.playvideo').hide();
        $('.playvideo').children().last().remove();
    });


    if ($("#entityInfo_relatedList_size").val() == 0) {
        $('.related').hide();
    }

    $('.related-list span').click(function () {
        var searchWord = $(this).text().trim();
        searchWord=replaceSpecialCharacters(searchWord);
        sessionStorage.setItem('searchWord', searchWord);
        sessionStorage.setItem('isExactSearch', 'true');
        //已经通过静态url生成a标签点击可以直接跳转,不需要再绑定点击跳转,会影响计数器准确性

    });
    // 得分
    $.ajax({
        url: $("#contextPath").val() + '/baike/getScoreByTopic?lang=' + lang + '&domain=0&topic=' + $("#entityInfo_name").val(),
        type: 'GET',
        success: function (res) {
            const point = res.data;
            $('.answer').eq(3).text(point);
            for (let i = 0; i < Math.floor(point); i++) {
                $('.statistic-list').eq(3).append(checkXssHTML(
                    `<img src="${$("#contextPath").val()}/pages/EncyclopediaJsp/result/img/star.png">`
                ))
            }
            if (point !== Math.floor(point)) {
                $('.statistic-list').eq(3).append(checkXssHTML(
                    `<img src="${$("#contextPath").val()}/pages/EncyclopediaJsp/result/img/star_half.png">`
                ))
            }
        }
    })

});
function playvideoHover(t){
    $(t).find('.close-box').addClass('active')
}
function playvideoLeave(t){
    $(t).find('.close-box').removeClass('active')
}
function initSearchInputDetail(language){
    //初始化搜索
    var searchPluginConfig = {
        selector : "#search-inputbox",
        lang:language,
        widthSelector:"#search-count",
        appendTo: "#search-count",
        searchPlaceholder:$("#search-inputbox").val(),
        isLogin:'false',
        whichPage:'pcDetail'
    }
    searchPlugin.init(searchPluginConfig);
}
// 联想
function suggest() {
    if ($input.val().trim() && $input.val().trim().length >= 2) {
        $.ajax({
            contentType: 'application/json',
            type: "POST",
            data: JSON.stringify({lang:lang,domain:domain,keyword:$input.val().trim()}),
            url: $("#contextPath").val() + '/baike/queryMatchedEntityList',
            dataType: "json",
            success: function (res) {
                const matchedList = res.data ? res.data.matchedList : []
                if (matchedList.length) {
                    pcDetail.isExist = true;
                    if (matchedList.length > 10) {
                        matchedList.splice(10)
                    }
                } else {
                    pcDetail.isExist = false;
                }
                $('.suggest').css({'max-height': '280px', border: '1px solid #333333', 'border-top': 'none'});
                let str = '';
                for (let val of matchedList) {
                    str += `<li>${val.name}</li>`
                }
                $('.suggest').html(checkXssHTML(str));
            }
        })
    } else {
        $('.suggest').css({'max-height': 0, border: 'none'});
    }
};
/*给email分享增加emailTo 链接*/
function addEmailTo(pageTitle,abstract,tip){
    var newTitle = pageTitle.replace(/&/g,'%26');
    var newStr = strTrunc(abstract);
    var URL = 'mailto:?subject='+newTitle+'&body='+ newStr +'%0d%0a%0d%0a'+ tip + encodeURIComponent(window.location.href);
    $('.email a,.email2 a').attr('href',URL);
}
function mbString(s) {
    var totalLength = 0;
    var i;
    var charCode;
    var seat = 0;
    for (i = 0; i < s.length; i++) {
        charCode = s.charCodeAt(i);
        if (charCode < 0x007f) {
            totalLength = totalLength + 1;
        } else if ((0x0080 <= charCode) && (charCode <= 0x07ff)) {
            totalLength += 2;
        } else if ((0x0800 <= charCode) && (charCode <= 0xffff)) {
            totalLength += 3;
        }
        //如果总长度大于400,则返回该位置
        if(totalLength > 400){
            seat = i;
            break;
        }
    }
    return seat;
}
function strTrunc(s){
    var count = 0;
    for (var i = 0; i < s.length; i++) {
        count += encodeURIComponent(s.charAt(i)).length;
        if (count > 1400) return s.substring(0, i - 1) + "...";
    };
    return s;
};

function replaceSpecialCharacters(keyword){
    var entityNameMappingWord=JSON.parse($('#entityNameMappingWord').val());
    $.each(entityNameMappingWord,function(i,v){
        if(keyword.indexOf(i)>-1){
            keyword=keyword.replace(i,v);
        }

    })
    return keyword
}
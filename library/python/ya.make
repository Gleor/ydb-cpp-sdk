OWNER(g:python-contrib)

RECURSE(
    aho_corasick
    aho_corasick/ut
    archive
    archive/benchmark
    archive/test
    archive/test/data
    asgi_yauth
    async_clients
    auth_client_parser
    awssdk-extensions
    awssdk_async_extensions
    base64
    base64/test
    bclclient
    blackbox
    blackbox/tests
    blackbox/tvm2
    bloom
    boost_test
    bstr
    build_info
    build_info/ut
    capabilities
    celery_dashboard
    certifi
    cgroups
    charset
    charts_notes
    charts_notes/example
    cityhash
    cityhash/test
    clickhouse_client
    cmain
    codecs
    codecs/gen_corpus
    codecs/test
    compress
    compress/tests
    cookiemy
    coredump_filter
    cores
    coverage
    cpp_test
    cppdemangle
    cqueue
    crowd-kit
    cyson
    cyson/pymodule
    cyson/ut
    deploy_formatter
    deprecated
    dir-sync
    django
    django/example
    django-idm-api
    django-multic
    django-sform
    django-sform/tests
    django_alive
    django_celery_monitoring
    django_russian
    django_template_common
    django_tools_log_context
    dssclient
    dump_dict
    edit_distance
    errorboosterclient
    filelock
    filelock/ut
    filesys
    filesys/ut
    find_root
    flask
    flask_passport
    fnvhash
    fnvhash/test
    framing
    framing/ut
    func
    func/ut
    fs
    geolocation
    geolocation/ut
    geohash
    geohash/ut
    golovan_stats_aggregator
    granular_settings
    granular_settings/tests
    guid
    guid/test
    guid/at_fork_test
    gunicorn
    hnsw
    ids
    import_test
    infected_masks
    infected_masks/ut
    init_log
    init_log/example
    intrasearch_fetcher
    json
    json/compare
    json/perf
    json/test
    json/test/py2
    json/test/py3
    langdetect
    langmask
    langs
    luigi
    luigi/data
    luigi/example
    luigi/luigid_static
    maths
    messagebus
    metrics_framework
    mime_types
    monitoring
    monlib
    monlib/examples
    monlib/so
    murmurhash
    nirvana
    nirvana_api
    nirvana_api/test_lib
    nirvana_test
    nstools
    nyt
    oauth
    oauth/example
    ok_client
    openssl
    par_apply
    par_apply/test
    path
    path/tests
    protobuf
    pymain
    pyscopg2
    pytest
    pytest-mongodb
    pytest/allure
    pytest/empty
    pytest/plugins
    python-blackboxer
    python-django-tanker
    python-django-yauth/tests
    python-django-yauth
    reactor
    redis_utils
    reservoir_sampling
    refsclient
    resource
    retry
    retry/tests
    runtime
    runtime/main
    runtime/test
    runtime_py3
    runtime_py3/main
    runtime_py3/test
    runtime_test
    sanitizers
    sdms_api
    sfx
    selenium_ui_test
    sendmsg
    stubmaker
    solomon
    spack
    spyt
    ssh_client
    ssh_sign
    startrek_python_client
    startrek_python_client/tests_int
    statface_client
    step
    strings
    strings/ut
    svn_ssh
    svn_version
    svn_version/ut
    symbols
    testing
    tmp
    toloka_client
    toloka-kit
    toloka-airflow
    toloka-prefect
    tools_structured_logs
    thread
    thread/test
    tskv
    tvmauth
    tvm2
    tvm2/tests
    type_info
    type_info/test
    unique_id
    vault_client 
    watch_dog
    watch_dog/example
    wiki
    windows
    windows/ut
    yandex_tracker_client
    yenv
    yt
    yt/test
    ylock
    ylock/tests
    zipatch
)

IF (NOT MUSL)
    RECURSE(
        yt/example
    )
ENDIF()

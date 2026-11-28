# patches

对**上游依赖**打的补丁。构建时由 CI 克隆上游、应用这里的补丁（见 `.github/workflows/build.yml`
的「Clone upstream sing-box and apply our patch」步骤）。

## sing-box-0001-platform-log-disable-colors.patch

- 目标：`SagerNet/sing-box`，版本由 `build.yml` 的 `SING_BOX_VERSION` 钉住（当前 v1.14.6）
- 内容：平台日志（journald / 系统日志）用的 formatter 关掉 ANSI 颜色。
  上游 `log/observable.go` 里那句

  ```go
  /*if platformWriter != nil {
      factory.platformFormatter.DisableColors = platformWriter.DisableColors()
  }*/
  ```

  是被注释掉的，所以平台日志会带上颜色转义符；补丁在 `NewDefaultFactory` 的
  `platformFormatter` literal 里直接置 `DisableColors: true`。
- 为什么是补丁而不是 fork：这个改动总共 1 行。为此维护一个完整的 sing-box fork
  （10MB、每次升上游都要重推/rebase、且改动藏在 fork 的历史里看不出来）不划算；
  fork 还叫 `sing-box`，会碰到上游 LICENSE 里的**名称限制条款**（未经同意不得在衍生作品中
  使用 sing-box 名称）。补丁放在本仓库，改动公开可见，升级只需改 `SING_BOX_VERSION`。
- **升级内核的流程**：

  ```sh
  # 1) 改 build.yml 的 SING_BOX_VERSION
  #    同时看新 tag 的 sing-box/go.mod 里的 go 指令，必要时抬 CI 的 go-version
  #    （v1.14.6 起 sing-box 要求 go >= 1.26.0，CI 已固定 1.26.11）
  # 2) 本地验证补丁仍然干净
  git clone --depth 1 --branch <新 tag> https://github.com/SagerNet/sing-box.git /tmp/sb
  git -C /tmp/sb apply --check "$PWD/patches/sing-box-0001-platform-log-disable-colors.patch"
  # 3) 双平台编译：先用 -mod=mod 补齐平台相关依赖的 go.sum 条目，再用默认的
  #    -mod=readonly 复验（CI 就是这个模式；只在一个平台编会漏掉另一平台的 sum）
  cd go/cmd/sboxxd_core
  GOOS=linux   go build -mod=mod -tags "<CI 同款 tags>" .
  GOOS=windows go build -mod=mod -tags "<CI 同款 tags>" .
  GOOS=linux   go build -tags "<CI 同款 tags>" .   # readonly 复验
  GOOS=windows go build -tags "<CI 同款 tags>" .
  # 4) 用新版 sing-box CLI 对「真实生成的配置」跑 check
  #    上游补丁版也会加严格校验，例如：
  #      v1.14.5 起校验 DNS 规则引用的 server（会报 dns rule[N]: server not found）
  #      v1.14.5 起校验路由规则引用的 outbound
  go run ./cmd/sing-box check -c <导出的运行配置>.json
  ```

  打不上就按新上下文重做补丁；CI 里 `git apply` 失败会**当场**失败，不会静默用未打补丁的
  内核发版；打完还会 grep 自检那一行确实落在 `log/observable.go` 里。

  版本历史：v1.14.6 的补丁需按上游新上下文重做（上游把 `DisableColors` 的自动设置整段注释掉了，
  `PlatformWriter` 接口也不再有该方法，所以仍然需要在 formatter 里硬置 true）；
  v1.14.5 新增的 DNS server 校验曾让「测试配置引用 dns-direct」全部报错，修见 5.0.26。

#pragma once

#include "main/SboxXD.hpp"
#include "db/traffic/TrafficData.hpp"
#include "fmt/AbstractBean.hpp"

namespace SboxXD_fmt {
    class SocksHttpBean;

    class ShadowSocksBean;

    class VMessBean;

    class TrojanVLESSBean;

    class NaiveBean;

    class QUICBean;

    class CustomBean;

    class ChainBean;

    class WireGuardBean;

    class SSHBean;

    class AnyTLSBean;

    class BrookBean;
}; // namespace SboxXD_fmt

namespace SboxXD {
    class ProxyEntity : public JsonStore {
    public:
        QString type;

        int id = -1;
        int gid = 0;
        int latency = 0;
        std::shared_ptr<SboxXD_fmt::AbstractBean> bean;
        std::shared_ptr<SboxXD_traffic::TrafficData> traffic_data = std::make_shared<SboxXD_traffic::TrafficData>("");

        QString full_test_report;

        ProxyEntity(SboxXD_fmt::AbstractBean *bean, const QString &type_);

        [[nodiscard]] QString DisplayLatency() const;

        [[nodiscard]] QColor DisplayLatencyColor() const;

        [[nodiscard]] SboxXD_fmt::ChainBean *ChainBean() const {
            return (SboxXD_fmt::ChainBean *) bean.get();
        };

        [[nodiscard]] SboxXD_fmt::SocksHttpBean *SocksHTTPBean() const {
            return (SboxXD_fmt::SocksHttpBean *) bean.get();
        };

        [[nodiscard]] SboxXD_fmt::ShadowSocksBean *ShadowSocksBean() const {
            return (SboxXD_fmt::ShadowSocksBean *) bean.get();
        };

        [[nodiscard]] SboxXD_fmt::VMessBean *VMessBean() const {
            return (SboxXD_fmt::VMessBean *) bean.get();
        };

        [[nodiscard]] SboxXD_fmt::TrojanVLESSBean *TrojanVLESSBean() const {
            return (SboxXD_fmt::TrojanVLESSBean *) bean.get();
        };

        [[nodiscard]] SboxXD_fmt::NaiveBean *NaiveBean() const {
            return (SboxXD_fmt::NaiveBean *) bean.get();
        };

        [[nodiscard]] SboxXD_fmt::QUICBean *QUICBean() const {
            return (SboxXD_fmt::QUICBean *) bean.get();
        };

        [[nodiscard]] SboxXD_fmt::CustomBean *CustomBean() const {
            return (SboxXD_fmt::CustomBean *) bean.get();
        };

        [[nodiscard]] SboxXD_fmt::WireGuardBean *WireGuardBean() const {
            return (SboxXD_fmt::WireGuardBean *) bean.get();
        };

        [[nodiscard]] SboxXD_fmt::SSHBean *SSHBean() const {
            return (SboxXD_fmt::SSHBean *) bean.get();
        };

        [[nodiscard]] SboxXD_fmt::AnyTLSBean *AnyTLSBean() const {
            return (SboxXD_fmt::AnyTLSBean *) bean.get();
        };

        [[nodiscard]] SboxXD_fmt::BrookBean *BrookBean() const {
            return (SboxXD_fmt::BrookBean *) bean.get();
        };
    };
} // namespace SboxXD

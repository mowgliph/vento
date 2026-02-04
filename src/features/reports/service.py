from typing import List, Dict
from datetime import datetime, timedelta
from dataclasses import dataclass
from src.features.sales.models import Sale
from src.features.sales.service import SaleService
from src.features.products.service import ProductService
from src.features.exchange_rate.service import ExchangeRateService

@dataclass
class SalesReport:
    """Sales report data structure"""
    sales: List[Sale]
    total_usd: float
    total_cup: float
    total_converted_to_usd: float
    total_converted_to_cup: float
    product_summary: Dict[int, Dict]

class ReportService:
    """Service for generating reports"""
    
    def __init__(self, 
                 sale_service: SaleService = None,
                 product_service: ProductService = None,
                 exchange_service: ExchangeRateService = None):
        self._sale_service = sale_service or SaleService()
        self._product_service = product_service or ProductService()
        self._exchange_service = exchange_service or ExchangeRateService()
    
    def generate_sales_report(self, start_date: datetime, end_date: datetime) -> SalesReport:
        """Generate sales report for date range"""
        sales = self._sale_service.get_by_date_range(start_date, end_date)
        return self._calculate_report(sales)
    
    def generate_daily_report(self, date: datetime = None) -> SalesReport:
        """Generate daily sales report"""
        if date is None:
            date = datetime.now()
        return self.generate_sales_report(
            date.replace(hour=0, minute=0, second=0, microsecond=0),
            date.replace(hour=23, minute=59, second=59, microsecond=999999)
        )
    
    def generate_product_sales_report(self, product_id: int) -> List[Sale]:
        """Generate report for specific product"""
        return self._sale_service.get_by_product(product_id)
    
    def _calculate_report(self, sales: List[Sale]) -> SalesReport:
        """Calculate totals and summary from sales list"""
        rate = self._exchange_service.get_current_rate()
        
        total_usd = 0.0
        total_cup = 0.0
        product_summary = {}
        
        for sale in sales:
            amount = sale.sale_price * sale.quantity
            
            if sale.sale_currency == "USD":
                total_usd += amount
            else:
                total_cup += amount
            
            # Product summary
            if sale.product_id not in product_summary:
                product_summary[sale.product_id] = {
                    'product_name': sale.product_name,
                    'quantity_sold': 0,
                    'total_usd': 0.0,
                    'total_cup': 0.0
                }
            
            summary = product_summary[sale.product_id]
            summary['quantity_sold'] += sale.quantity
            if sale.sale_currency == "USD":
                summary['total_usd'] += amount
            else:
                summary['total_cup'] += amount
        
        total_converted_to_usd = total_usd + (total_cup / rate.usd_to_cup)
        total_converted_to_cup = total_cup + (total_usd * rate.usd_to_cup)
        
        return SalesReport(
            sales=sales,
            total_usd=total_usd,
            total_cup=total_cup,
            total_converted_to_usd=total_converted_to_usd,
            total_converted_to_cup=total_converted_to_cup,
            product_summary=product_summary
        )
    
    def export_to_text(self, report: SalesReport) -> str:
        """Export report to text format for printing"""
        lines = [
            "=" * 60,
            "           REPORTE DE VENTAS - VENTO",
            "=" * 60,
            f"Fecha: {datetime.now().strftime('%d/%m/%Y %H:%M')}",
            "",
            "RESUMEN:",
            f"  Total en USD: ${report.total_usd:.2f}",
            f"  Total en CUP: ${report.total_cup:.2f} MN",
            f"  Total convertido a USD: ${report.total_converted_to_usd:.2f}",
            f"  Total convertido a CUP: ${report.total_converted_to_cup:.2f} MN",
            "",
            "-" * 60,
            "DETALLE POR PRODUCTO:",
            "-" * 60
        ]
        
        for product_id, summary in report.product_summary.items():
            lines.extend([
                f"  {summary['product_name']}",
                f"    Cantidad: {summary['quantity_sold']}",
                f"    USD: ${summary['total_usd']:.2f} | CUP: ${summary['total_cup']:.2f} MN",
                ""
            ])
        
        lines.extend([
            "-" * 60,
            "DETALLE DE VENTAS:",
            "-" * 60
        ])
        
        for sale in report.sales:
            lines.append(
                f"  {sale.sale_date.strftime('%d/%m/%Y %H:%M')} - "
                f"{sale.product_name} - "
                f"{sale.quantity}x ${sale.sale_price:.2f} {sale.sale_currency}"
            )
        
        lines.extend(["", "=" * 60, "Fin del reporte", "=" * 60])
        
        return "\n".join(lines)
